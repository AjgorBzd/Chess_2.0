#ifndef CHESSGAME_H
#define CHESSGAME_H

#include <QObject>
#include "ChessBoard.h"
#include "Player.h"

class ChessGame : public QObject
{
    Q_OBJECT

private:
    ChessBoard board;
    PieceColor currentTurn;
    Player playerWhite{PieceColor::White, "Gracz 1", ":/images/default_avatar_white.png"};
    Player playerBlack{PieceColor::Black, "Gracz 2", ":/images/default_avatar_black.png"};
public:
    explicit ChessGame(QObject *parent = nullptr);
    void startNewGame();
    PieceColor getCurrentTurn() const { return currentTurn; }
    void switchTurn();

    PieceType getPieceTypeAt(int row, int col) const;
    PieceColor getPieceColorAt(int row, int col) const;
    Player& getPlayer(PieceColor color) {
        return (color == PieceColor::White) ? playerWhite : playerBlack;
    }
    bool attemptMove(int fromRow, int fromCol, int toRow, int toCol);

signals:

public slots:

};

#endif // CHESSGAME_H
