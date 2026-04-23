#pragma once
#include <QObject>
#include "GameSettings.h"
#include <QTimer>

class ChessGame;
class MainWindow;

class GameController : public QObject
{
    Q_OBJECT
public:
    GameController(ChessGame *model, MainWindow *view, QObject *parent = nullptr);
    GameSettings getSettings() const { return currentSettings; }
    void applySettings(const GameSettings& newSettings);
    QTimer* m_matchTimer;

public slots:
    void handlePlayPlayerRequest();

    void handleSettingsOpenRequest(bool isMidGame);
    void handleSettingsSaved(GameSettings newSettings);

    void handleSquareClicked(int row, int col);
    void onTimerTick();

private:
    ChessGame *m_model;
    MainWindow *m_view;
    void syncBoardToView();
    GameSettings currentSettings;

    bool m_isPieceSelected = false;
    int m_selectedRow = -1;
    int m_selectedCol = -1;
};