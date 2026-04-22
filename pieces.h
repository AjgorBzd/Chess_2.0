#pragma once
#include <vector>
#include "ChessEnums.h"

struct Position {
    int row;
    int col;
};

class ChessBoard;

class Piece {
protected:
    PieceColor color;
    bool moved = false;

public:
    Piece(PieceColor c) : color(c) {}
    virtual ~Piece() = default;

    virtual PieceType getType() const = 0;

    PieceColor getColor() const { return color; }

    bool hasMoved() const { return moved; }
    void setMoved(bool m) { moved = m; }

};

class EmptyPiece : public Piece {
public:
    EmptyPiece() : Piece(PieceColor::None) {}
    PieceType getType() const override { return PieceType::Empty; }
};

class Pawn : public Piece {
public:
    Pawn(PieceColor c) : Piece(c) {}
    PieceType getType() const override { return PieceType::Pawn; }
};

class Knight : public Piece {
public:
    Knight(PieceColor c) : Piece(c) {}
    PieceType getType() const override { return PieceType::Knight; }
};

class Bishop : public Piece {
public:
    Bishop(PieceColor c) : Piece(c) {}
    PieceType getType() const override { return PieceType::Bishop; }
};

class Rook : public Piece {
public:
    Rook(PieceColor c) : Piece(c) {}
    PieceType getType() const override { return PieceType::Rook; }
};

class Queen : public Piece {
public:
    Queen(PieceColor c) : Piece(c) {}
    PieceType getType() const override { return PieceType::Queen; }
};

class King : public Piece {
public:
    King(PieceColor c) : Piece(c) {}
    PieceType getType() const override { return PieceType::King; }
};