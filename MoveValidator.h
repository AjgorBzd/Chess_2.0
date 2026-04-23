#pragma once
#include <vector>
#include <memory>
#include "ChessEnums.h"
#include "ChessBoard.h"
#include "pieces.h"


class MoveValidator {
public:
    static std::vector<LegalMove> getLegalMoves(int row, int col, ChessBoard& board);

    static bool isKingInCheck(PieceColor kingColor, const ChessBoard& board);
    static CheckInfo getCheckInfo(PieceColor kingColor, const ChessBoard& board);

private:
    static bool doesMoveLeaveKingInCheck(int fromRow, int fromCol, int toRow, int toCol, ChessBoard& board);
};