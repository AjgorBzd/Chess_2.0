#pragma once
#include <QDialog>
#include "GameSettings.h"

namespace Ui { class SettingsDialog; }

class SettingsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(const GameSettings& currentSettings, bool isMidGame, QWidget *parent = nullptr);
    ~SettingsDialog();

    GameSettings getSettings() const;

private slots:
    void on_chk_handicap_toggled(bool checked);
    void on_btn_save_clicked();
    void on_btn_cancel_clicked();
    void on_spin_p1Time_valueChanged(int arg1);
    void on_spin_p1Inc_valueChanged(int arg1);

    void on_btn_p1Avatar_clicked();
    void on_btn_p2Avatar_clicked();

private:
    Ui::SettingsDialog *ui;
    GameSettings m_settings;
};