#include "trayicon.h"

#include <QApplication>
#include <QIcon>
#include <QMenu>

TrayIcon::TrayIcon(QWidget* parentWindow)
    : QSystemTrayIcon(parentWindow)
    , m_parent(parentWindow) {
    setIcon(QIcon::fromTheme(QStringLiteral("applications-development"),
                             QIcon(QStringLiteral(":/icons/app.png"))));
    setToolTip(QStringLiteral("Web Launcher"));

    QMenu* menu = new QMenu();
    menu->addAction(QStringLiteral("🪟 Hiện cửa sổ"), this, [this] { emit showRequested(); });
    menu->addSeparator();
    menu->addAction(QStringLiteral("🚪 Thoát"), qApp, &QApplication::quit);
    setContextMenu(menu);

    connect(this, &QSystemTrayIcon::activated, this, [this](QSystemTrayIcon::ActivationReason reason) {
        if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick)
            emit showRequested();
    });

    show();
}
