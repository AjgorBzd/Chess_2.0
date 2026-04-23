#include "GameController.h"
#include "ChessGame.h"
#include "mainwindow.h"

GameController::GameController(ChessGame *model, MainWindow *view, QObject *parent)
    : QObject{parent}, m_model(model), m_view(view)
{
    connect(m_view, &MainWindow::requestPlayPlayer, this, &GameController::handlePlayPlayerRequest);
    connect(m_view, &MainWindow::requestSettingsOpen, this, &GameController::handleSettingsOpenRequest);
    connect(m_view, &MainWindow::settingsSaved, this, &GameController::handleSettingsSaved);

    connect(m_view, &MainWindow::squareClicked, this, &GameController::handleSquareClicked);
    m_matchTimer = new QTimer(this);
    connect(m_matchTimer, &QTimer::timeout, this, &GameController::onTimerTick);
    connect(m_view, &MainWindow::requestUndoMove, this, &GameController::handleUndoMoveRequest);

    m_view->applySettingsToUI(currentSettings);
}

void GameController::onTimerTick() {
    if (m_model->getGameState() != GameState::Active) return;
    m_model->decrementCurrentTimer();

    int p1Time = m_model->getPlayer(PieceColor::White).getTimeLeft();
    int p2Time = m_model->getPlayer(PieceColor::Black).getTimeLeft();
    m_view->updateTimers(p1Time, p2Time);

    if (p1Time <= 0) {
        m_matchTimer->stop();
        m_model->setGameState(GameState::BlackWins);
        m_model->setGameStarted(false);
        m_view->showGameOverDialog(GameState::BlackWins, currentSettings);
    } else if (p2Time <= 0) {
        m_matchTimer->stop();
        m_model->setGameState(GameState::WhiteWins);
        m_model->setGameStarted(false);
        m_view->showGameOverDialog(GameState::WhiteWins, currentSettings);
    }
}

void GameController::handlePlayPlayerRequest()
{
    if (m_matchTimer->isActive()) {
        m_matchTimer->stop();
    }
    m_model->startNewGame(currentSettings);


    // 2. Command the View to change the screen and wipe the history box
    m_view->showGamePage();
    m_view->drawCoordinates(m_model->getCurrentTurn());
    int p1Time = m_model->getPlayer(PieceColor::White).getTimeLeft();
    int p2Time = m_model->getPlayer(PieceColor::Black).getTimeLeft();
    m_view->RestartUI(p1Time, p2Time);

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
    currentSettings = newSettings;
    m_view->applySettingsToUI(currentSettings);

    bool shouldBeFlipped = (currentSettings.autoFlipBoard && m_model->getCurrentTurn() == PieceColor::Black);
    m_view->setFlipped(shouldBeFlipped);

    m_isPieceSelected = false;
    m_selectedRow = -1;
    m_selectedCol = -1;
    m_view->clearHighlights();

    syncBoardToView();

    CheckInfo checkInfo = m_model->getCurrentCheckInfo();
    if (checkInfo.inCheck) {
        m_view->highlightCheck(checkInfo);
    }
}

void GameController::handleSquareClicked(int row, int col)
{
    // qDebug() << "Controller registered click at Row:" << row << "Col:" << col;

    // PieceColor clickedColor = m_model->getPieceColorAt(row, col);
    // PieceType clickedType = m_model->getPieceTypeAt(row, col);

    // switch(clickedType)
    // {
    //     case PieceType::Empty:
    //         qDebug() << "You didn't click on a piece.";
    //         break;
    //     case PieceType::Pawn:
    //         qDebug() << "You clicked on a pawn.";
    //         break;
    //     case PieceType::Bishop:
    //         qDebug() << "You clicked on a bishop.";
    //         break;
    //     case PieceType::Knight:
    //         qDebug() << "You clicked on a knight.";
    //         break;
    //     case PieceType::Rook:
    //         qDebug() << "You clicked on a rook.";
    //         break;
    //     case PieceType::Queen:
    //         qDebug() << "You clicked on a queen.";
    //         break;
    //     case PieceType::King:
    //         qDebug() << "You clicked on a king.";
    //         break;
    // }

    if (m_model->getGameState() != GameState::Active) return;

    if (!m_isPieceSelected) {
        if (m_model->canPickUp(row, col)) {
            m_isPieceSelected = true;
            m_selectedRow = row;
            m_selectedCol = col;

            auto moves = m_model->getLegalMovesForPiece(row, col);
            m_view->highlightMoves(moves);

            PieceType type = m_model->getPieceTypeAt(row, col);
            PieceColor color = m_model->getPieceColorAt(row, col);
            m_view->pickUpPiece(row, col, color, type);
        }
    } else {
        m_view->dropPiece();
        PieceType movingPiece = m_model->getPieceTypeAt(m_selectedRow, m_selectedCol);
        PieceType promotionChoice = PieceType::Empty;

        if (movingPiece == PieceType::Pawn && (row == 0 || row == 7)) {
            auto legalMoves = m_model->getLegalMovesForPiece(m_selectedRow, m_selectedCol);
            bool isLegal = false;
            for (const auto& m : legalMoves) {
                if (m.row == row && m.col == col) { isLegal = true; break; }
            }

            if (isLegal) {
                m_view->setSquare(m_selectedRow, m_selectedCol, PieceColor::None, PieceType::Empty);
                m_view->setSquare(row, col, m_model->getCurrentTurn(), PieceType::Pawn);
                syncBoardToView();

                promotionChoice = m_view->showPromotionDialog(m_model->getCurrentTurn(), row, col);

                if (promotionChoice == PieceType::Empty) {
                    m_isPieceSelected = false;
                    m_selectedRow = -1;
                    m_selectedCol = -1;
                    m_view->clearHighlights();
                    syncBoardToView();
                    return;
                }
            }
        }
        bool moveSuccessful = m_model->attemptMove(m_selectedRow, m_selectedCol, row, col, promotionChoice);

        if (moveSuccessful && !m_model->hasGameStarted()) {
            m_model->setGameStarted(true);
            m_matchTimer->start(1000);
        }

        m_isPieceSelected = false;
        m_selectedRow = -1;
        m_selectedCol = -1;

        m_view->clearHighlights();

        if (currentSettings.autoFlipBoard && m_model->getCurrentTurn() == PieceColor::Black) {
            m_view->setFlipped(true);
        } else {
            m_view->setFlipped(false);
        }

        syncBoardToView();

        m_view->updateHistory(m_model->getHistory());

        Player& w = m_model->getPlayer(PieceColor::White);
        Player& b = m_model->getPlayer(PieceColor::Black);

        // Calculate Advantage
        int wAdv = std::max(0, w.getMaterialScore() - b.getMaterialScore());
        int bAdv = std::max(0, b.getMaterialScore() - w.getMaterialScore());

        m_view->updateCaptures(w.getCapturedPieces(), b.getCapturedPieces(), wAdv, bAdv);

        // Highlights
        CheckInfo checkInfo = m_model->getCurrentCheckInfo();
        if (checkInfo.inCheck) {
            m_view->highlightCheck(checkInfo);
        }

        GameState state = m_model->getGameState();
        if (state != GameState::Active) {
            m_matchTimer->stop();
            m_model->setGameStarted(false);

            syncBoardToView();

            m_view->showGameOverDialog(state, currentSettings);
        }

        m_view->updateTimers(m_model->getPlayer(PieceColor::White).getTimeLeft(),
                             m_model->getPlayer(PieceColor::Black).getTimeLeft());
    }
}

void GameController::handleUndoMoveRequest() {
    if (m_model->undoLastMove()) {

        m_isPieceSelected = false;
        m_selectedRow = -1;
        m_selectedCol = -1;
        m_view->clearHighlights();

        m_view->updateTimers(m_model->getPlayer(PieceColor::White).getTimeLeft(),
                             m_model->getPlayer(PieceColor::Black).getTimeLeft());

        bool shouldBeFlipped = (currentSettings.autoFlipBoard && m_model->getCurrentTurn() == PieceColor::Black);
        m_view->setFlipped(shouldBeFlipped);
        m_view->drawCoordinates(shouldBeFlipped ? PieceColor::Black : PieceColor::White);

        syncBoardToView();

        m_view->updateHistory(m_model->getHistory());

        Player& w = m_model->getPlayer(PieceColor::White);
        Player& b = m_model->getPlayer(PieceColor::Black);
        int wAdv = std::max(0, w.getMaterialScore() - b.getMaterialScore());
        int bAdv = std::max(0, b.getMaterialScore() - w.getMaterialScore());
        m_view->updateCaptures(w.getCapturedPieces(), b.getCapturedPieces(), wAdv, bAdv);

        if (currentSettings.autoFlipBoard && m_model->getCurrentTurn() == PieceColor::Black) {
            m_view->setFlipped(true);
        } else {
            m_view->setFlipped(false);
        }
        CheckInfo checkInfo = m_model->getCurrentCheckInfo();
        if (checkInfo.inCheck) {
            m_view->highlightCheck(checkInfo);
        }
        if (m_model->getHistory().empty()) {
            m_matchTimer->stop();
            m_model->setGameStarted(false);
        }
    }
}