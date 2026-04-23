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
    int timeLeft;

public:
    Player(PieceColor c, const std::string& defaultName, const std::string& defaultAvatar, int time)
        : color(c), name(defaultName), avatarPath(defaultAvatar), timeLeft(time) {}

    PieceColor getColor() const { return color; }

    std::string getName() const { return name; }
    void setName(const std::string& newName) { name = newName; }

    std::string getAvatarPath() const { return avatarPath; }
    void setAvatarPath(const std::string& newPath) { avatarPath = newPath; }

    void addCapturedPiece(PieceType piece) { capturedPieces.push_back(piece); }
    const std::vector<PieceType>& getCapturedPieces() const { return capturedPieces; }
    void clearCapturedPieces() { capturedPieces.clear(); }

    int getTimeLeft() const { return timeLeft; }
    void setTimeLeft(int seconds) { timeLeft = seconds; }
    void decrementTime() { if (timeLeft > 0) timeLeft--; }

    int getMaterialScore() const {
        int score = 0;
        for (PieceType type : capturedPieces) {
            if (type == PieceType::Pawn) score += 1;
            else if (type == PieceType::Knight || type == PieceType::Bishop) score += 3;
            else if (type == PieceType::Rook) score += 5;
            else if (type == PieceType::Queen) score += 9;
        }
        return score;
    }

    void popCapturedPiece() {
        if (!capturedPieces.empty()) capturedPieces.pop_back();
    }
};