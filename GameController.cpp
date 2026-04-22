#include "GameController.h"
#include "ChessGame.h"
#include "mainwindow.h"

GameController::GameController(ChessGame *model, MainWindow *view, QObject *parent)
    : QObject{parent}, m_model(model), m_view(view)
{
    connect(m_view, &MainWindow::requestPlayPlayer, this, &GameController::handlePlayPlayerRequest);
    connect(m_view, &MainWindow::requestSettingsOpen, this, &GameController::handleSettingsOpenRequest);
    connect(m_view, &MainWindow::settingsSaved, this, &GameController::handleSettingsSaved);

    m_view->applySettingsToUI(currentSettings);
}

void GameController::handlePlayPlayerRequest()
{
    // 1. Tell the Model to start a fresh game
    m_model->startNewGame();


    // 2. Command the View to change the screen and wipe the history box
    m_view->showGamePage();
    m_view->drawCoordinates(m_model->getCurrentTurn());
    m_view->clearHistoryUI();

    // 3. Push the new board state to the View
    syncBoardToView();
}

void GameController::syncBoardToView()
{
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            PieceColor color = m_model->getPieceColorAt(row, col);
            PieceType type = m_model->getPieceTypeAt(row, col);
            m_view->setSquare(row, col, color, type);
        }
    }
}

void GameController::handleSettingsOpenRequest(bool isMidGame) {
    // Controller commands the View to open the dialog and feeds it the current data
    m_view->openSettingsDialog(currentSettings, isMidGame);
}

void GameController::handleSettingsSaved(GameSettings newSettings) {
    // 1. Save the new truth
    currentSettings = newSettings;

    // 2. Update the Models (we'll expand this later to actually update the Player objects in ChessGame)
    // m_model->getPlayer(PieceColor::White).setName(m_settings.p1Name);

    // 3. Command the View to refresh the names and timers
    m_view->applySettingsToUI(currentSettings);
}