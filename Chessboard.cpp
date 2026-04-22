#include "ChessBoard.h"


ChessBoard::ChessBoard() {
    setupStandardBoard();
}

void ChessBoard::setupStandardBoard() {
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            board[row][col] = std::make_shared<EmptyPiece>();
        }
    }

    board[0][0] = std::make_shared<Rook>(PieceColor::Black);
    board[0][1] = std::make_shared<Knight>(PieceColor::Black);
    board[0][2] = std::make_shared<Bishop>(PieceColor::Black);
    board[0][3] = std::make_shared<Queen>(PieceColor::Black);
    board[0][4] = std::make_shared<King>(PieceColor::Black);
    board[0][5] = std::make_shared<Bishop>(PieceColor::Black);
    board[0][6] = std::make_shared<Knight>(PieceColor::Black);
    board[0][7] = std::make_shared<Rook>(PieceColor::Black);

    for (int col = 0; col < 8; ++col) {
        board[1][col] = std::make_shared<Pawn>(PieceColor::Black);
    }

    for (int col = 0; col < 8; ++col) {
        board[6][col] = std::make_shared<Pawn>(PieceColor::White);
    }

    board[7][0] = std::make_shared<Rook>(PieceColor::White);
    board[7][1] = std::make_shared<Knight>(PieceColor::White);
    board[7][2] = std::make_shared<Bishop>(PieceColor::White);
    board[7][3] = std::make_shared<Queen>(PieceColor::White);
    board[7][4] = std::make_shared<King>(PieceColor::White);
    board[7][5] = std::make_shared<Bishop>(PieceColor::White);
    board[7][6] = std::make_shared<Knight>(PieceColor::White);
    board[7][7] = std::make_shared<Rook>(PieceColor::White);
}

std::shared_ptr<Piece> ChessBoard::getPieceAt(int row, int col) const {
    if (row < 0 || row >= 8 || col < 0 || col >= 8) {
        return std::make_shared<EmptyPiece>();
    }
    return board[row][col];
}

void ChessBoard::movePiece(int fromRow, int fromCol, int toRow, int toCol) {
    board[toRow][toCol] = board[fromRow][fromCol];

    board[toRow][toCol]->setMoved(true);

    board[fromRow][fromCol] = std::make_shared<EmptyPiece>();
}