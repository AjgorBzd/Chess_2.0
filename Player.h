#pragma once
#include <string>
#include <vector>
#include "ChessEnums.h"

class Player {
private:
    PieceColor color;
    std::string name;
    std::string avatarPath;
    std::vector<PieceType> capturedPieces;

public:
    Player(PieceColor c, const std::string& defaultName, const std::string& defaultAvatar)
        : color(c), name(defaultName), avatarPath(defaultAvatar) {}

    PieceColor getColor() const { return color; }

    std::string getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }

    std::string getAvatarPath() const { return avatarPath; }
    void setAvatarPath(const std::string& newPath) { avatarPath = newPath; }

    void addCapturedPiece(PieceType piece) { capturedPieces.push_back(piece); }
    const std::vector<PieceType>& getCapturedPieces() const { return capturedPieces; }
    void clearCapturedPieces() { capturedPieces.clear(); }
};