#pragma once

#include <QDialog>

class QSettings;
class QCheckBox;

// Hộp thoại cài đặt đơn giản.
class SettingsDialog : public QDialog {
    Q_OBJECT
public:
    explicit SettingsDialog(QSettings* settings, QWidget* parent = nullptr);

private:
    QSettings* m_settings = nullptr;
    QCheckBox* m_dark = nullptr;
    QCheckBox* m_tray = nullptr;
};
