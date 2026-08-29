#pragma once

#include <QHash>
#include <QMainWindow>
#include <QVector>

class QListWidget;
class QListWidgetItem;
class QTabWidget;
class QLabel;
class QSettings;
class QTermWidget;

#include "core/database.h"
#include "core/project.h"

class TrayIcon;
class SettingsDialog;

// Cửa sổ chính: thanh công cụ, danh sách dự án, tab terminal.
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

private slots:
    void addFolder();              // thêm 1 thư mục dự án
    void addFolderRecursive();     // quét đệ quy 1 thư mục
    void refreshList();
    void onItemActivated(QListWidgetItem* item);
    void runProject(const Project& p);
    void stopProject(const Project& p);
    void openInBrowser(const Project& p);
    void openInFileManager(const Project& p);
    void showSettings();
    void toggleFavorite();
    void removeProjectFromList();
    void closeTab(int index);
    void onProjectStopped(const QString& projectPath);
    void copyTerminalLog();
    void copyTerminalSelection();
    void pasteToTerminal();

protected:
    void closeEvent(QCloseEvent* event) override;
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    void setupUi();
    void reloadList();
    void openTerminal(const Project& p);
    void updateRunButtonState();
    void trackRecent(const Project& p);
    void applySettings();
    QTermWidget* currentTerminal() const;
    void copyHistoryToClipboard(QTermWidget* term);

    Database m_db;
    QListWidget* m_list = nullptr;
    QTabWidget* m_tabs = nullptr;
    QLabel* m_status = nullptr;
    QAction* m_runAction = nullptr;
    QAction* m_stopAction = nullptr;
    QAction* m_favAction = nullptr;
    QAction* m_removeAction = nullptr;
    QAction* m_copyLogAction = nullptr;
    TrayIcon* m_tray = nullptr;
    QSettings* m_settings = nullptr;
    QVector<Project> m_projects; // danh sách hiển thị
    QHash<QString, int> m_tabForPath; // đường dẫn -> index tab
};
