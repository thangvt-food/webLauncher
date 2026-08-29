#include "window.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QDateTime>
#include <QDesktopServices>
#include <QDir>
#include <QFileDialog>
#include <QFont>
#include <QInputDialog>
#include <QLabel>
#include <QListWidget>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPalette>
#include <QSettings>
#include <QStatusBar>
#include <QSplitter>
#include <QTabWidget>
#include <QTextCodec>
#include <QTimer>
#include <QToolBar>
#include <QUrl>
#include <QVBoxLayout>

#include <qtermwidget.h>

#include "core/util.h"
#include "scanner.h"
#include "settingsdialog.h"
#include "trayicon.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , m_settings(new QSettings(QStringLiteral("WebLauncher"), QStringLiteral("WebLauncher"), this)) {
    setupUi();
    m_db.load();
    reloadList();
    applySettings();
    restoreGeometry(m_settings->value(QStringLiteral("geometry")).toByteArray());
}

MainWindow::~MainWindow() {
    m_settings->setValue(QStringLiteral("geometry"), saveGeometry());
    // Đóng mọi tab terminal -> dừng dev server
    while (m_tabs->count() > 0)
        closeTab(0);
}

void MainWindow::setupUi() {
    setWindowTitle(tr("Web Launcher"));
    resize(980, 640);

    QToolBar* tb = addToolBar(tr("Chính"));
    tb->setMovable(false);
    m_runAction = tb->addAction(tr("▶ Chạy"), this, [this] {
        if (QListWidgetItem* item = m_list->currentItem())
            onItemActivated(item);
    });
    m_stopAction = tb->addAction(tr("■ Dừng"), this, [this] {
        if (QListWidgetItem* item = m_list->currentItem()) {
            const int row = m_list->row(item);
            if (row >= 0 && row < m_projects.size())
                stopProject(m_projects[row]);
        }
    });
    tb->addSeparator();
    m_favAction = tb->addAction(tr("☆ Yêu thích"), this, [this] { toggleFavorite(); });
    m_removeAction = tb->addAction(tr("🗑 Xóa"), this, [this] { removeProjectFromList(); });

    // Danh sách dự án (trái)
    m_list = new QListWidget(this);
    m_list->setSelectionMode(QAbstractItemView::SingleSelection);
    m_list->setContextMenuPolicy(Qt::CustomContextMenu);
    m_list->setMinimumWidth(300);
    connect(m_list, &QListWidget::itemActivated, this, &MainWindow::onItemActivated);
    connect(m_list, &QListWidget::currentRowChanged, this, [this](int) { updateRunButtonState(); });
    connect(m_list, &QListWidget::customContextMenuRequested, this, [this](const QPoint& pos) {
        QListWidgetItem* item = m_list->itemAt(pos);
        if (!item)
            return;
        const int row = m_list->row(item);
        if (row < 0 || row >= m_projects.size())
            return;
        const Project p = m_projects[row];
        QMenu menu(this);
        menu.addAction(tr("▶ Chạy"), this, [this, p] { runProject(p); });
        menu.addAction(tr("■ Dừng"), this, [this, p] { stopProject(p); });
        menu.addSeparator();
        menu.addAction(tr("🌐 Mở trình duyệt"), this, [this, p] { openInBrowser(p); });
        menu.addAction(tr("📂 Mở thư mục"), this, [this, p] { openInFileManager(p); });
        menu.addSeparator();
        menu.addAction(tr("☆/★ Yêu thích"), this, [this, p] { toggleFavorite(); });
        menu.addAction(tr("🗑 Xóa khỏi danh sách"), this, [this, p] { removeProjectFromList(); });
        menu.exec(m_list->viewport()->mapToGlobal(pos));
    });

    // Tab terminal (phải)
    m_tabs = new QTabWidget(this);
    m_tabs->setTabsClosable(true);
    m_tabs->setMovable(true);
    connect(m_tabs, &QTabWidget::tabCloseRequested, this, &MainWindow::closeTab);

    QSplitter* splitter = new QSplitter(Qt::Horizontal, this);
    splitter->addWidget(m_list);
    splitter->addWidget(m_tabs);
    splitter->setStretchFactor(0, 0);
    splitter->setStretchFactor(1, 1);
    splitter->setSizes({320, 660});
    setCentralWidget(splitter);

    // Menu
    QMenu* fileMenu = menuBar()->addMenu(tr("&Tệp"));
    fileMenu->addAction(tr("➕ Thêm thư mục dự án..."), this, [this] { addFolder(); });
    fileMenu->addAction(tr("➕ Quét thư mục (đệ quy)..."), this, [this] { addFolderRecursive(); });
    fileMenu->addSeparator();
    fileMenu->addAction(tr("⚙️ Cài đặt..."), this, [this] { showSettings(); });
    fileMenu->addSeparator();
    fileMenu->addAction(tr("🚪 Thoát"), this, [this] { close(); });

    QMenu* viewMenu = menuBar()->addMenu(tr("&Xem"));
    viewMenu->addAction(tr("🔄 Làm mới danh sách"), this, [this] { refreshList(); });
    viewMenu->addSeparator();
    viewMenu->addAction(tr("🌐 Mở trình duyệt"), this, [this] {
        if (QListWidgetItem* item = m_list->currentItem()) {
            const int row = m_list->row(item);
            if (row >= 0 && row < m_projects.size())
                openInBrowser(m_projects[row]);
        }
    });

    QMenu* helpMenu = menuBar()->addMenu(tr("&Trợ giúp"));
    helpMenu->addAction(tr("ℹ️ Giới thiệu"), this, [this] {
        QMessageBox::about(this, tr("Web Launcher"),
            tr("<h3>Web Launcher</h3>"
               "<p>Khởi chạy nhanh các dự án web (React, Next.js, Vite...) "
               "mà không cần gõ lệnh thủ công.</p>"
               "<p>Chọn dự án và bấm <b>▶ Chạy</b> — app tự mở terminal tích hợp "
               "và chạy lệnh dev server cho bạn.</p>"));
    });

    // Trạng thái
    m_status = new QLabel(this);
    statusBar()->addWidget(m_status);

    // Khay hệ thống
    m_tray = new TrayIcon(this);
    connect(m_tray, &TrayIcon::showRequested, this, [this] {
        show();
        raise();
        activateWindow();
    });

    updateRunButtonState();
}

void MainWindow::reloadList() {
    m_projects = m_db.projects();
    std::sort(m_projects.begin(), m_projects.end(), projectLessThan);
    m_list->clear();
    for (const Project& p : m_projects) {
        const QString script = p.runScript.isEmpty() ? QStringLiteral("?") : p.runScript;
        QString text = QStringLiteral("%1  [%2]").arg(p.displayName(), script);
        if (p.favorite)
            text.prepend(QStringLiteral("★ "));
        QListWidgetItem* item = new QListWidgetItem(text, m_list);
        item->setToolTip(p.path);
        item->setData(Qt::UserRole, p.id);
    }
    if (m_list->count() > 0)
        m_list->setCurrentRow(0);
    updateRunButtonState();
    m_status->setText(tr("%1 dự án").arg(m_projects.size()));
}

void MainWindow::addFolder() {
    const QString dir = QFileDialog::getExistingDirectory(this, tr("Chọn thư mục dự án"));
    if (dir.isEmpty())
        return;
    const Project p = ProjectScanner::scanDirectory(dir, m_db);
    if (p.id.isEmpty()) {
        QMessageBox::information(this, tr("Không tìm thấy"),
            tr("Thư mục này không có package.json."));
        return;
    }
    if (!m_db.addProject(p)) {
        QMessageBox::information(this, tr("Đã có"),
            tr("Dự án này đã có trong danh sách."));
        return;
    }
    reloadList();
}

void MainWindow::addFolderRecursive() {
    const QString root = QFileDialog::getExistingDirectory(this, tr("Chọn thư mục gốc để quét"));
    if (root.isEmpty())
        return;
    QApplication::setOverrideCursor(Qt::WaitCursor);
    const QVector<Project> found = ProjectScanner::scanRecursive(root, 3);
    QApplication::restoreOverrideCursor();

    int added = 0;
    for (const Project& p : found)
        if (m_db.addProject(p))
            ++added;
    QMessageBox::information(this, tr("Quét xong"),
        tr("Tìm thấy %1 dự án, đã thêm %2 dự án mới.").arg(found.size()).arg(added));
    reloadList();
}

void MainWindow::refreshList() {
    for (const Project& p : m_projects) {
        Project fresh = ProjectScanner::scanDirectory(p.path, m_db);
        if (!fresh.id.isEmpty()) {
            Project updated = fresh;
            updated.favorite = p.favorite;
            updated.lastRun = p.lastRun;
            m_db.updateProject(updated);
        }
    }
    reloadList();
}

void MainWindow::onItemActivated(QListWidgetItem* item) {
    const int row = m_list->row(item);
    if (row < 0 || row >= m_projects.size())
        return;
    runProject(m_projects[row]);
}

void MainWindow::runProject(const Project& p) {
    if (!m_tabForPath.contains(p.path)) {
        openTerminal(p);
        // Mở trình duyệt sau khi dev server kịp khởi động
        if (p.port > 0)
            QTimer::singleShot(1500, this, [this, p] { openInBrowser(p); });
    } else {
        m_tabs->setCurrentIndex(m_tabForPath.value(p.path));
    }
}

void MainWindow::openTerminal(const Project& p) {
    QTermWidget* term = new QTermWidget(this);
    term->setTerminalFont(QFont(QStringLiteral("monospace"), 11));
    term->setColorScheme(QStringLiteral("Linux"));
    term->setScrollBarPosition(QTermWidget::ScrollBarRight);
    term->setTextCodec(QTextCodec::codecForName("UTF-8"));

    const QString cmd = p.effectiveCommand();
    const QString line = QStringLiteral("cd %1 && clear && echo '▶ Đang chạy: %2' && %3\n")
                             .arg(util::shellQuote(p.path), cmd, cmd);
    term->sendText(line);
    term->setFocus();

    const int index = m_tabs->addTab(term, p.displayName());
    m_tabs->setCurrentIndex(index);
    m_tabForPath.insert(p.path, index);

    if (Project* dbp = m_db.find(p.id)) {
        dbp->lastRun = QDateTime::currentMSecsSinceEpoch();
        m_db.save();
    }

    // Shell kết thúc (server dừng) -> đóng tab tương ứng
    connect(term, &QTermWidget::finished, this, [this, p] {
        const int idx = m_tabForPath.value(p.path, -1);
        if (idx >= 0) {
            m_tabForPath.remove(p.path);
            m_tabs->removeTab(idx);
        }
        updateRunButtonState();
    });
    updateRunButtonState();
}

void MainWindow::closeTab(int index) {
    QWidget* w = m_tabs->widget(index);
    if (!w)
        return;
    QString path;
    for (auto it = m_tabForPath.constBegin(); it != m_tabForPath.constEnd(); ++it) {
        if (it.value() == index) {
            path = it.key();
            break;
        }
    }
    // Gửi exit cho shell trong tab (shell sẽ đóng -> finished -> dọn dẹp)
    QTermWidget* term = qobject_cast<QTermWidget*>(w);
    if (term)
        term->sendText(QStringLiteral("exit\n"));
    if (!path.isEmpty())
        m_tabForPath.remove(path);
    // Xóa tab ngay; tín hiệu finished cũng có thể tới sau đó
    m_tabs->removeTab(index);
    w->deleteLater();
    updateRunButtonState();
}

void MainWindow::stopProject(const Project& p) {
    if (!m_tabForPath.contains(p.path))
        return;
    closeTab(m_tabForPath.value(p.path));
}

void MainWindow::openInBrowser(const Project& p) {
    int port = p.port;
    if (port <= 0) {
        bool ok = false;
        port = QInputDialog::getInt(this, tr("Port"),
            tr("Nhập port của dev server:"), 3000, 1, 65535, 1, &ok);
        if (!ok)
            return;
        if (Project* dbp = m_db.find(p.id)) {
            dbp->port = port;
            m_db.save();
        }
    }
    QDesktopServices::openUrl(QUrl(QStringLiteral("http://localhost:%1").arg(port)));
}

void MainWindow::openInFileManager(const Project& p) {
    QDesktopServices::openUrl(QUrl::fromLocalFile(p.path));
}

void MainWindow::showSettings() {
    SettingsDialog dlg(m_settings, this);
    if (dlg.exec() == QDialog::Accepted)
        applySettings();
}

void MainWindow::toggleFavorite() {
    QListWidgetItem* item = m_list->currentItem();
    if (!item)
        return;
    const int row = m_list->row(item);
    if (row < 0 || row >= m_projects.size())
        return;
    Project* p = m_db.find(m_projects[row].id);
    if (!p)
        return;
    p->favorite = !p->favorite;
    m_db.save();
    reloadList();
}

void MainWindow::removeProjectFromList() {
    QListWidgetItem* item = m_list->currentItem();
    if (!item)
        return;
    const int row = m_list->row(item);
    if (row < 0 || row >= m_projects.size())
        return;
    const Project p = m_projects[row];
    if (QMessageBox::question(this, tr("Xóa dự án"),
            tr("Xóa \"%1\" khỏi danh sách? (không xóa file trên đĩa)")
                .arg(p.displayName())) != QMessageBox::Yes)
        return;
    m_db.removeProject(p.id);
    reloadList();
}

void MainWindow::updateRunButtonState() {
    const bool has = m_list->currentItem() != nullptr;
    m_runAction->setEnabled(has);
    m_stopAction->setEnabled(has);
    m_favAction->setEnabled(has);
    m_removeAction->setEnabled(has);
}

void MainWindow::applySettings() {
    const bool dark = m_settings->value(QStringLiteral("darkTheme"), false).toBool();
    qApp->setStyle(QStringLiteral("Fusion"));
    if (dark) {
        QPalette pal;
        pal.setColor(QPalette::Window, QColor(45, 45, 48));
        pal.setColor(QPalette::WindowText, Qt::white);
        pal.setColor(QPalette::Base, QColor(30, 30, 32));
        pal.setColor(QPalette::AlternateBase, QColor(45, 45, 48));
        pal.setColor(QPalette::Text, Qt::white);
        pal.setColor(QPalette::Button, QColor(60, 60, 64));
        pal.setColor(QPalette::ButtonText, Qt::white);
        pal.setColor(QPalette::Highlight, QColor(42, 130, 218));
        pal.setColor(QPalette::HighlightedText, Qt::white);
        qApp->setPalette(pal);
    } else {
        qApp->setPalette(qApp->style()->standardPalette());
    }
}

void MainWindow::onProjectStopped(const QString& projectPath) {
    Q_UNUSED(projectPath);
    updateRunButtonState();
}

QWidget* MainWindow::currentTabTerminal() const {
    return m_tabs->currentWidget();
}

void MainWindow::closeEvent(QCloseEvent* event) {
    const bool minimizeToTray = m_settings->value(QStringLiteral("minimizeToTray"), true).toBool();
    if (minimizeToTray && m_tray->isVisible()) {
        hide();
        event->ignore();
        return;
    }
    event->accept();
}
