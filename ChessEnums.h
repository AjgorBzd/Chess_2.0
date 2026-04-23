#pragma once
#include <vector>

struct Position {
    int row;
    int col;
};

enum class PieceColor {
    White,
    Black,
    None
};

enum class PieceType {
    Pawn,
    Knight,
    Bishop,
    Rook,
    Queen,
    King,
    Empty
};

struct LegalMove {
    int row;
    int col;
    bool isCapture;
};

struct CheckInfo {
    bool inCheck = false;
    Position kingPos = {-1, -1};
    std::vector<Position> attackers;
};

struct MoveRecord {
    int fromRow;
    int fromCol;
    int toRow;
    int toCol;
    PieceType pieceMoved;
    PieceColor playerColor;
    PieceType pieceCaptured;

    // We can easily expand this later!
    bool isCheck = false;
    bool isCheckmate = false;
    bool isShortCastling = false;
    bool isLongCastling = false;
    bool isPromotionToQueen = false;
    bool isPromotionToRook = false;
    bool isPromotionToKnight = false;
    bool isPromotionToBishop = false;

};

