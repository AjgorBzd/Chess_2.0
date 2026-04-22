#ifndef CHESSGAME_H
#define CHESSGAME_H

#include <QObject>
#include "ChessBoard.h"

class ChessGame : public QObject
{
    Q_OBJECT

private:
    ChessBoard board;
    PieceColor currentTurn;
public:
    explicit ChessGame(QObject *parent = nullptr);
    void startNewGame();
    PieceColor getCurrentTurn() const { return currentTurn; }
    void switchTurn();

    PieceType getPieceTypeAt(int row, int col) const;
    PieceColor getPieceColorAt(int row, int col) const;

signals:

public slots:

};

#endif // CHESSGAME_H
