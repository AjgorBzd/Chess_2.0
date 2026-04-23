#include "MoveValidator.h"

std::vector<LegalMove> MoveValidator::getLegalMoves(int row, int col, ChessBoard& board) {
    std::vector<LegalMove> legalMoves;

    auto piece = board.getPieceAt(row, col);
    if (piece->getType() == PieceType::Empty) {
        return legalMoves;
    }

    std::vector<Position> basicMoves = piece->getBasicLegalMoves(row, col, board);

    for (const auto& move : basicMoves) {
        if (!doesMoveLeaveKingInCheck(row, col, move.row, move.col, board)) {

            bool capture = (board.getPieceAt(move.row, move.col)->getType() != PieceType::Empty);
            legalMoves.push_back({move.row, move.col, capture});
        }
    }

    return legalMoves;
}

bool MoveValidator::isKingInCheck(PieceColor kingColor, const ChessBoard& board) {
    int kingRow = -1, kingCol = -1;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            auto p = board.getPieceAt(r, c);
            if (p->getType() == PieceType::King && p->getColor() == kingColor) {
                kingRow = r;
                kingCol = c;
                break;
            }
        }
        if (kingRow != -1) break;
    }

    if (kingRow == -1) return false;

    PieceColor enemyColor = (kingColor == PieceColor::White) ? PieceColor::Black : PieceColor::White;

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            auto p = board.getPieceAt(r, c);
            if (p->getColor() == enemyColor) {
                auto enemyMoves = p->getBasicLegalMoves(r, c, board);
                for (const auto& em : enemyMoves) {
                    if (em.row == kingRow && em.col == kingCol) {
                        return true;
                    }
                }
            }
        }
    }
    return false;
}

CheckInfo MoveValidator::getCheckInfo(PieceColor kingColor, const ChessBoard& board) {
    CheckInfo info;
    info.inCheck = false;

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            auto p = board.getPieceAt(r, c);
            if (p->getType() == PieceType::King && p->getColor() == kingColor) {
                info.kingPos = {r, c};
                break;
            }
        }
        if (info.kingPos.row != -1) break;
    }

    if (info.kingPos.row == -1) return info;

    PieceColor enemyColor = (kingColor == PieceColor::White) ? PieceColor::Black : PieceColor::White;

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            auto p = board.getPieceAt(r, c);
            if (p->getColor() == enemyColor) {

                auto enemyMoves = p->getBasicLegalMoves(r, c, board);
                for (const auto& em : enemyMoves) {
                    if (em.row == info.kingPos.row && em.col == info.kingPos.col) {
                        info.inCheck = true;
                        info.attackers.push_back({r, c});
                        break;
                    }
                }
            }
        }
    }

    return info;
}

bool MoveValidator::doesMoveLeaveKingInCheck(int fromRow, int fromCol, int toRow, int toCol, ChessBoard& board) {

    auto movingPiece = board.getPieceAt(fromRow, fromCol);
    auto targetPiece = board.getPieceAt(toRow, toCol);
    PieceColor myColor = movingPiece->getColor();
    board.setPieceAt(toRow, toCol, movingPiece);
    board.setPieceAt(fromRow, fromCol, std::make_shared<EmptyPiece>());
    bool inCheck = isKingInCheck(myColor, board);

    board.setPieceAt(fromRow, fromCol, movingPiece);
    board.setPieceAt(toRow, toCol, targetPiece);

    return inCheck;
}