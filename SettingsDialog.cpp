#include "settingsdialog.h"
#include "ui_SettingsDialog.h"
#include <QFileDialog>

SettingsDialog::SettingsDialog(const GameSettings& currentSettings, bool isMidGame, QWidget *parent)
    : QDialog(parent), ui(new Ui::SettingsDialog), m_settings(currentSettings)
{
    ui->setupUi(this);
    this->setWindowFlags(Qt::FramelessWindowHint | Qt::Dialog);

    ui->chk_autoFlip->setChecked(m_settings.autoFlipBoard);
    ui->chk_handicap->setChecked(m_settings.handicapMode);
    ui->spin_p1Time->setValue(m_settings.p1BaseTimeSeconds / 60);
    ui->spin_p1Inc->setValue(m_settings.p1IncrementSeconds);
    ui->spin_p2Time->setValue(m_settings.p2BaseTimeSeconds / 60);
    ui->spin_p2Inc->setValue(m_settings.p2IncrementSeconds);

    ui->edit_p1Name->setText(QString::fromStdString(m_settings.p1Name));
    ui->edit_p2Name->setText(QString::fromStdString(m_settings.p2Name));

    ui->lbl_p1Avatar->setPixmap(QPixmap(QString::fromStdString(m_settings.p1AvatarPath)).scaled(34, 34, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    ui->lbl_p2Avatar->setPixmap(QPixmap(QString::fromStdString(m_settings.p2AvatarPath)).scaled(34, 34, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));

    if (isMidGame) {
        ui->grp_timeSettings->setEnabled(false);
    } else {
        on_chk_handicap_toggled(m_settings.handicapMode);
    }
}

SettingsDialog::~SettingsDialog() {
    delete ui;
}

void SettingsDialog::on_chk_handicap_toggled(bool checked) {
    ui->spin_p2Time->setEnabled(checked);
    ui->spin_p2Inc->setEnabled(checked);
    if (!checked) {
        ui->spin_p2Time->setValue(ui->spin_p1Time->value());
        ui->spin_p2Inc->setValue(ui->spin_p1Inc->value());
    }
}

void SettingsDialog::on_btn_save_clicked() {
    m_settings.autoFlipBoard = ui->chk_autoFlip->isChecked();
    m_settings.handicapMode = ui->chk_handicap->isChecked();

    m_settings.p1BaseTimeSeconds = ui->spin_p1Time->value() * 60;
    m_settings.p1IncrementSeconds = ui->spin_p1Inc->value();

    if (!m_settings.handicapMode) {
        m_settings.p2BaseTimeSeconds = m_settings.p1BaseTimeSeconds;
        m_settings.p2IncrementSeconds = m_settings.p1IncrementSeconds;
    } else {
        m_settings.p2BaseTimeSeconds = ui->spin_p2Time->value() * 60;
        m_settings.p2IncrementSeconds = ui->spin_p2Inc->value();
    }

    m_settings.p1Name = ui->edit_p1Name->text().toStdString();
    m_settings.p2Name = ui->edit_p2Name->text().toStdString();

    this->accept();
}

void SettingsDialog::on_btn_cancel_clicked() {
    this->reject();
}

GameSettings SettingsDialog::getSettings() const {
    return m_settings;
}

void SettingsDialog::on_spin_p1Time_valueChanged(int arg1)
{
    if (!ui->chk_handicap->isChecked()) {
        ui->spin_p2Time->setValue(arg1);
    }
}

void SettingsDialog::on_spin_p1Inc_valueChanged(int arg1)
{
    if (!ui->chk_handicap->isChecked()) {
        ui->spin_p2Inc->setValue(arg1);
    }
}

void SettingsDialog::on_btn_p1Avatar_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Wybierz Avatar", "", "Images (*.png *.jpg *.jpeg)");
    if (!fileName.isEmpty()) {
        m_settings.p1AvatarPath = fileName.toStdString();
        ui->lbl_p1Avatar->setPixmap(QPixmap(fileName).scaled(34, 34, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    }
}

void SettingsDialog::on_btn_p2Avatar_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Wybierz Avatar", "", "Images (*.png *.jpg *.jpeg)");
    if (!fileName.isEmpty()) {
        m_settings.p2AvatarPath = fileName.toStdString();
        ui->lbl_p2Avatar->setPixmap(QPixmap(fileName).scaled(34, 34, Qt::KeepAspectRatioByExpanding, Qt::SmoothTransformation));
    }
}