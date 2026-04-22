#pragma once
#include <string>

struct GameSettings {
    bool autoFlipBoard = false;

    int p1BaseTimeSeconds = 300;
    int p1IncrementSeconds = 0;

    int p2BaseTimeSeconds = 300;
    int p2IncrementSeconds = 0;

    bool handicapMode = false;

    // --- Player Identifiers ---
    std::string p1Name = "Gracz 1";
    std::string p1AvatarPath = ":/images/default_avatar_white.png";

    std::string p2Name = "Gracz 2";
    std::string p2AvatarPath = ":/images/default_avatar_black.png";
};