#pragma once
#include <memory>
#include "pieces.h"

class ChessBoard {
private:
    std::shared_ptr<Piece> board[8][8];

public:
    ChessBoard();
    ~ChessBoard() = default;

    void setupStandardBoard();

    std::shared_ptr<Piece> getPieceAt(int row, int col) const;

    void movePiece(int fromRow, int fromCol, int toRow, int toCol);
    void setPieceAt(int row, int col, std::shared_ptr<Piece> piece) {
        board[row][col] = piece;
    }
};