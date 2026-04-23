#include "pieces.h"
#include "ChessBoard.h"

std::vector<Position> Knight::getBasicLegalMoves(int myRow, int myCol, const ChessBoard& board) const {
    std::vector<Position> moves;

    int rowOffsets[] = {-2, -2, -1, -1,  1,  1,  2,  2};
    int colOffsets[] = {-1,  1, -2,  2, -2,  2, -1,  1};

    for (int i = 0; i < 8; ++i) {
        int targetRow = myRow + rowOffsets[i];
        int targetCol = myCol + colOffsets[i];

        if (targetRow >= 0 && targetRow < 8 && targetCol >= 0 && targetCol < 8) {

            PieceColor targetColor = board.getPieceAt(targetRow, targetCol)->getColor();
            if (targetColor != this->color) {
                moves.push_back({targetRow, targetCol});
            }
        }
    }
    return moves;
}

std::vector<Position> Rook::getBasicLegalMoves(int myRow, int myCol, const ChessBoard& board) const {
    std::vector<Position> moves;

    int rowDirs[] = {-1, 1, 0, 0};
    int colDirs[] = {0, 0, -1, 1};

    for (int i = 0; i < 4; ++i) {
        int currentRow = myRow + rowDirs[i];
        int currentCol = myCol + colDirs[i];

        while (currentRow >= 0 && currentRow < 8 && currentCol >= 0 && currentCol < 8) {

            PieceColor targetColor = board.getPieceAt(currentRow, currentCol)->getColor();

            if (targetColor == PieceColor::None) {
                moves.push_back({currentRow, currentCol});
            }
            else if (targetColor != this->color) {
                moves.push_back({currentRow, currentCol});
                break;
            }
            else {
                break;
            }

            currentRow += rowDirs[i];
            currentCol += colDirs[i];
        }
    }
    return moves;
}

std::vector<Position> Bishop::getBasicLegalMoves(int myRow, int myCol, const ChessBoard& board) const {
    std::vector<Position> moves;

    int rowDirs[] = {-1, -1, 1, 1};
    int colDirs[] = {-1, 1, -1, 1};

    for (int i = 0; i < 4; ++i) {
        int currentRow = myRow + rowDirs[i];
        int currentCol = myCol + colDirs[i];

        while (currentRow >= 0 && currentRow < 8 && currentCol >= 0 && currentCol < 8) {

            PieceColor targetColor = board.getPieceAt(currentRow, currentCol)->getColor();

            if (targetColor == PieceColor::None) {
                moves.push_back({currentRow, currentCol});
            }
            else if (targetColor != this->color) {
                moves.push_back({currentRow, currentCol});
                break;
            }
            else {
                break;
            }

            currentRow += rowDirs[i];
            currentCol += colDirs[i];
        }
    }
    return moves;
}

std::vector<Position> Queen::getBasicLegalMoves(int myRow, int myCol, const ChessBoard& board) const {
    std::vector<Position> moves;

    // West, North-West, North, North-East, East, South-East, South, South-West
    int rowDirs[] = {-1, -1, 0, 1, 1,  1,  0, -1};
    int colDirs[] = { 0,  1, 1, 1, 0, -1, -1, -1};

    for (int i = 0; i < 8; ++i) {
        int currentRow = myRow + rowDirs[i];
        int currentCol = myCol + colDirs[i];

        while (currentRow >= 0 && currentRow < 8 && currentCol >= 0 && currentCol < 8) {

            PieceColor targetColor = board.getPieceAt(currentRow, currentCol)->getColor();

            if (targetColor == PieceColor::None) {
                moves.push_back({currentRow, currentCol});
            }
            else if (targetColor != this->color) {
                moves.push_back({currentRow, currentCol});
                break;
            }
            else {
                break;
            }

            currentRow += rowDirs[i];
            currentCol += colDirs[i];
        }
    }
    return moves;
}

std::vector<Position> King::getBasicLegalMoves(int myRow, int myCol, const ChessBoard& board) const {
    std::vector<Position> moves;

    int rowOffsets[] = {-1, -1, 0, 1, 1,  1,  0, -1};
    int colOffsets[] = { 0,  1, 1, 1, 0, -1, -1, -1};

    for (int i = 0; i < 8; ++i) {
        int targetRow = myRow + rowOffsets[i];
        int targetCol = myCol + colOffsets[i];

        if (targetRow >= 0 && targetRow < 8 && targetCol >= 0 && targetCol < 8) {

            PieceColor targetColor = board.getPieceAt(targetRow, targetCol)->getColor();
            if (targetColor != this->color) {
                moves.push_back({targetRow, targetCol});
            }
        }
    }
    return moves;
}

std::vector<Position> Pawn::getBasicLegalMoves(int myRow, int myCol, const ChessBoard& board) const {
    std::vector<Position> moves;

    int dir = (this->color == PieceColor::White) ? -1 : 1;

    int forwardRow = myRow + dir;
    if (forwardRow >= 0 && forwardRow < 8) {
        if (board.getPieceAt(forwardRow, myCol)->getType() == PieceType::Empty) {
            moves.push_back({forwardRow, myCol});

            int doubleRow = myRow + (2 * dir);
            if (!this->hasMoved() && doubleRow >= 0 && doubleRow < 8) {
                if (board.getPieceAt(doubleRow, myCol)->getType() == PieceType::Empty) {
                    moves.push_back({doubleRow, myCol});
                }
            }
        }
    }

    int captureCols[] = {myCol - 1, myCol + 1};
    for (int col : captureCols) {
        if (forwardRow >= 0 && forwardRow < 8 && col >= 0 && col < 8) {
            PieceColor targetColor = board.getPieceAt(forwardRow, col)->getColor();
            // Must be an enemy piece to capture (cannot be empty or friendly)
            if (targetColor != PieceColor::None && targetColor != this->color) {
                moves.push_back({forwardRow, col});
            }
        }
    }

    return moves;
}