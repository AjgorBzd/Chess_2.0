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

    board.movePiece(fromRow, fromCol, toRow, toCol);
    return true;
}