#include "ChessGame.h"

ChessGame::ChessGame(QObject *parent)
    : QObject{parent}
{}

void ChessGame::startNewGame() {
    board.setupStandardBoard();
    currentTurn = PieceColor::White;
    // moveHistory.clear(); // Will add when we build the history struct
}

PieceType ChessGame::getPieceTypeAt(int row, int col) const {
    return board.getPieceAt(row, col)->getType();
}

PieceColor ChessGame::getPieceColorAt(int row, int col) const {
    return board.getPieceAt(row, col)->getColor();
}

void ChessGame::switchTurn() {
    currentTurn = (currentTurn == PieceColor::White) ? PieceColor::Black : PieceColor::White;
}

bool ChessGame::attemptMove(int fromRow, int fromCol, int toRow, int toCol) {

    if (fromRow == toRow && fromCol == toCol) {
        return false;
    }

    PieceColor targetColor = getPieceColorAt(toRow, toCol);
    if (targetColor == currentTurn) {
        return false;
    }

    board.movePiece(fromRow, fromCol, toRow, toCol);
    switchTurn();

    return true;
}

bool ChessGame::canPickUp(int row, int col) const {
    PieceType type = getPieceTypeAt(row, col);
    PieceColor color = getPieceColorAt(row, col);

    // You can only pick it up if it's not empty, AND it belongs to the current player
    return (type != PieceType::Empty && color == currentTurn);
}