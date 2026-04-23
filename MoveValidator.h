#pragma once
#include <vector>
#include "ChessEnums.h"
#include "Chessboard.h"
#include "pieces.h"


class MoveValidator {
public:
    static std::vector<LegalMove> getLegalMoves(int row, int col, ChessBoard& board, const std::vector<MoveRecord>& history);
    static bool hasAnyLegalMoves(PieceColor color, ChessBoard& board, const std::vector<MoveRecord>& history);

    static bool isKingInCheck(PieceColor kingColor, const ChessBoard& board);
    static CheckInfo getCheckInfo(PieceColor kingColor, const ChessBoard& board);

private:
    static bool doesMoveLeaveKingInCheck(int fromRow, int fromCol, int toRow, int toCol, ChessBoard& board, bool isEnPassant = false);
};