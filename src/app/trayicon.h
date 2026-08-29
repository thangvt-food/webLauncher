#pragma once

#include <QSystemTrayIcon>

// Khay hệ thống: thu nhỏ app, mở lại nhanh, menu ngữ cảnh.
class TrayIcon : public QSystemTrayIcon {
    Q_OBJECT
public:
    explicit TrayIcon(QWidget* parentWindow);

signals:
    void showRequested();

private:
    QWidget* m_parent = nullptr;
};
