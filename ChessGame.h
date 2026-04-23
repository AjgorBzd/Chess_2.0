#ifndef CHESSGAME_H
#define CHESSGAME_H

#include <QObject>
#include "Chessboard.h"
#include "Player.h"
#include "MoveValidator.h"
#include "GameSettings.h"
#include "ChessEnums.h"

class ChessGame : public QObject
{
    Q_OBJECT

private:
    ChessBoard board;
    PieceColor currentTurn;
    GameState currentState = GameState::Active;

    Player playerWhite{PieceColor::White, "Gracz 1", ":/images/default_avatar_white.png", 300};
    Player playerBlack{PieceColor::Black, "Gracz 2", ":/images/default_avatar_black.png", 300};

    std::vector<MoveRecord> moveHistory;
    bool gameStarted = false;

public:
    explicit ChessGame(QObject *parent = nullptr);
    void startNewGame(const GameSettings& settings);
    PieceColor getCurrentTurn() const { return currentTurn; }
    void switchTurn();

    PieceType getPieceTypeAt(int row, int col) const;
    PieceColor getPieceColorAt(int row, int col) const;
    Player& getPlayer(PieceColor color) {
        return (color == PieceColor::White) ? playerWhite : playerBlack;
    }
    bool attemptMove(int fromRow, int fromCol, int toRow, int toCol, PieceType promotionPiece = PieceType::Empty);
    bool canPickUp(int row, int col) const;

    std::vector<LegalMove> getLegalMovesForPiece(int row, int col);
    CheckInfo getCurrentCheckInfo() const;
    std::vector<MoveRecord> getHistory() const { return moveHistory; }

    bool hasGameStarted() const { return gameStarted; }
    void setGameStarted(bool started) { gameStarted = started; }
    void decrementCurrentTimer() { getPlayer(currentTurn).decrementTime(); }

    void applySettings(const GameSettings& settings);

    bool undoLastMove();

    GameState getGameState() const { return currentState; }
    void setGameState(GameState state) { currentState = state; }

signals:

public slots:

};

#endif // CHESSGAME_H
