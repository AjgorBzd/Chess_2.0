#pragma once
#include <QObject>
#include "GameSettings.h"

class ChessGame;
class MainWindow;

class GameController : public QObject
{
    Q_OBJECT
public:
    GameController(ChessGame *model, MainWindow *view, QObject *parent = nullptr);
    GameSettings getSettings() const { return currentSettings; }
    void applySettings(const GameSettings& newSettings);

public slots:
    void handlePlayPlayerRequest();

    void handleSettingsOpenRequest(bool isMidGame);
    void handleSettingsSaved(GameSettings newSettings);

private:
    ChessGame *m_model;
    MainWindow *m_view;
    void syncBoardToView();
    GameSettings currentSettings;
};