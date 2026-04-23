#include "MoveValidator.h"

std::vector<LegalMove> MoveValidator::getLegalMoves(int row, int col, ChessBoard& board, const std::vector<MoveRecord>& history) {
    std::vector<LegalMove> legalMoves;

    auto piece = board.getPieceAt(row, col);
    if (piece->getType() == PieceType::Empty) {
        return legalMoves;
    }
    PieceColor myColor = piece->getColor();
    std::vector<Position> basicMoves = piece->getBasicLegalMoves(row, col, board);

    for (const auto& move : basicMoves) {
        if (!doesMoveLeaveKingInCheck(row, col, move.row, move.col, board)) {
            bool capture = (board.getPieceAt(move.row, move.col)->getType() != PieceType::Empty);
            legalMoves.push_back({move.row, move.col, capture});
        }
    }

    // EN PASSANT
        if (piece->getType() == PieceType::Pawn && !history.empty()) {
        const MoveRecord& lastMove = history.back();
        if (lastMove.pieceMoved == PieceType::Pawn && std::abs(lastMove.fromRow - lastMove.toRow) == 2) {
            if (lastMove.toRow == row && std::abs(lastMove.toCol - col) == 1) {
                int dir = (myColor == PieceColor::White) ? -1 : 1;
                int epRow = row + dir;
                int epCol = lastMove.toCol;

                if (!doesMoveLeaveKingInCheck(row, col, epRow, epCol, board, true)) {
                    legalMoves.push_back({epRow, epCol, true, true});
                }
            }
        }
    }

    // CASTLING
    if (piece->getType() == PieceType::King && !piece->hasMoved() && !isKingInCheck(myColor, board)) {
        auto rookRight = board.getPieceAt(row, 7);

        if (rookRight->getType() == PieceType::Rook && !rookRight->hasMoved()) {
            if (board.getPieceAt(row, 5)->getType() == PieceType::Empty &&
                board.getPieceAt(row, 6)->getType() == PieceType::Empty) {

                if (!doesMoveLeaveKingInCheck(row, col, row, 5, board) &&
                    !doesMoveLeaveKingInCheck(row, col, row, 6, board)) {
                    legalMoves.push_back({row, 6, false, true});
                }
            }
        }


        auto rookLeft = board.getPieceAt(row, 0);
        if (rookLeft->getType() == PieceType::Rook && !rookLeft->hasMoved()) {
            if (board.getPieceAt(row, 1)->getType() == PieceType::Empty &&
                board.getPieceAt(row, 2)->getType() == PieceType::Empty &&
                board.getPieceAt(row, 3)->getType() == PieceType::Empty) {
                if (!doesMoveLeaveKingInCheck(row, col, row, 3, board) &&
                    !doesMoveLeaveKingInCheck(row, col, row, 2, board)) {
                    legalMoves.push_back({row, 2, false, true});
                }
            }
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

bool MoveValidator::doesMoveLeaveKingInCheck(int fromRow, int fromCol, int toRow, int toCol, ChessBoard& board, bool isEnPassant) {

    auto movingPiece = board.getPieceAt(fromRow, fromCol);
    auto targetPiece = board.getPieceAt(toRow, toCol);
    auto epPawn = board.getPieceAt(fromRow, toCol);

    PieceColor myColor = movingPiece->getColor();

    board.setPieceAt(toRow, toCol, movingPiece);
    board.setPieceAt(fromRow, fromCol, std::make_shared<EmptyPiece>());
    if (isEnPassant) board.setPieceAt(fromRow, toCol, std::make_shared<EmptyPiece>());

    bool inCheck = isKingInCheck(myColor, board);

    board.setPieceAt(fromRow, fromCol, movingPiece);
    board.setPieceAt(toRow, toCol, targetPiece);
    if (isEnPassant) board.setPieceAt(fromRow, toCol, epPawn);

    return inCheck;
}

bool MoveValidator::hasAnyLegalMoves(PieceColor color, ChessBoard& board, const std::vector<MoveRecord>& history) {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            auto piece = board.getPieceAt(r, c);
            if (piece->getColor() == color) {
                auto moves = getLegalMoves(r, c, board, history);
                if (!moves.empty()) return true;
            }
        }
    }
    return false;
}

bool MoveValidator::isInsufficientMaterial(const ChessBoard& board) {
    int whiteMinors = 0, blackMinors = 0;

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            auto p = board.getPieceAt(r, c);
            PieceType type = p->getType();

            if (type == PieceType::Empty || type == PieceType::King) continue;

            if (type == PieceType::Pawn || type == PieceType::Rook || type == PieceType::Queen) {
                return false;
            }

            if (p->getColor() == PieceColor::White) {
                whiteMinors++;
            } else if (p->getColor() == PieceColor::Black) {
                blackMinors++;
            }
        }
    }

    if (whiteMinors == 0 && blackMinors == 0) return true;
    if (whiteMinors == 1 && blackMinors == 0) return true;
    if (whiteMinors == 0 && blackMinors == 1) return true;

    return false;
}