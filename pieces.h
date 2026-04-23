#pragma once
#include <vector>
#include "ChessEnums.h"


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

    virtual std::vector<Position> getBasicLegalMoves(int myRow, int myCol, const ChessBoard& board) const = 0;

};

class EmptyPiece : public Piece {
public:
    EmptyPiece() : Piece(PieceColor::None) {}
    PieceType getType() const override { return PieceType::Empty; }
    std::vector<Position> getBasicLegalMoves(int, int, const ChessBoard&) const override { return {}; }
};

class Pawn : public Piece {
public:
    Pawn(PieceColor c) : Piece(c) {}
    PieceType getType() const override { return PieceType::Pawn; }
    std::vector<Position> getBasicLegalMoves(int myRow, int myCol, const ChessBoard& board) const override;
};

class Knight : public Piece {
public:
    Knight(PieceColor c) : Piece(c) {}
    PieceType getType() const override { return PieceType::Knight; }
    std::vector<Position> getBasicLegalMoves(int myRow, int myCol, const ChessBoard& board) const override;
};

class Bishop : public Piece {
public:
    Bishop(PieceColor c) : Piece(c) {}
    PieceType getType() const override { return PieceType::Bishop; }
    std::vector<Position> getBasicLegalMoves(int myRow, int myCol, const ChessBoard& board) const override;
};

class Rook : public Piece {
public:
    Rook(PieceColor c) : Piece(c) {}
    PieceType getType() const override { return PieceType::Rook; }
    std::vector<Position> getBasicLegalMoves(int myRow, int myCol, const ChessBoard& board) const override;
};

class Queen : public Piece {
public:
    Queen(PieceColor c) : Piece(c) {}
    PieceType getType() const override { return PieceType::Queen; }
    std::vector<Position> getBasicLegalMoves(int myRow, int myCol, const ChessBoard& board) const override;
};

class King : public Piece {
public:
    King(PieceColor c) : Piece(c) {}
    PieceType getType() const override { return PieceType::King; }
    std::vector<Position> getBasicLegalMoves(int myRow, int myCol, const ChessBoard& board) const override;
};