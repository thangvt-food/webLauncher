#include "settingsdialog.h"

#include <QCheckBox>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QSettings>
#include <QVBoxLayout>

SettingsDialog::SettingsDialog(QSettings* settings, QWidget* parent)
    : QDialog(parent)
    , m_settings(settings) {
    setWindowTitle(tr("Cài đặt"));
    setMinimumWidth(380);

    m_dark = new QCheckBox(tr("Giao diện tối"), this);
    m_dark->setChecked(m_settings->value(QStringLiteral("darkTheme"), false).toBool());

    m_tray = new QCheckBox(tr("Thu nhỏ vào khay hệ thống khi đóng cửa sổ"), this);
    m_tray->setChecked(m_settings->value(QStringLiteral("minimizeToTray"), true).toBool());

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    connect(buttons, &QDialogButtonBox::accepted, this, [this] {
        m_settings->setValue(QStringLiteral("darkTheme"), m_dark->isChecked());
        m_settings->setValue(QStringLiteral("minimizeToTray"), m_tray->isChecked());
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    QFormLayout* form = new QFormLayout();
    form->addRow(m_dark);
    form->addRow(m_tray);

    QVBoxLayout* lay = new QVBoxLayout(this);
    lay->addLayout(form);
    lay->addWidget(buttons);
}
