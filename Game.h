#ifndef GAME_H
#define GAME_H

#include "Board.h"
#include "PlayerManager.h"
#include "ReplayManager.h"
#include "Bot.h"
#include <vector>
#include <utility>

class Game {
private:
    Board board;
    PlayerManager& playerManager;
    ReplayManager& replayManager;

    // Current game state
    std::vector<std::pair<int, int>> moveHistory;
    
    // Helpers
    bool getCoordinates(int& r, int& c, const std::string& playerName);
    void printWinnerHUD(const std::string& winnerName, char symbol) const;
    void printDrawHUD() const;

public:
    Game(PlayerManager& pm, ReplayManager& rm);

    // Game modes
    void startPvP();
    void startPvE(int botDifficulty);
};

#endif // GAME_H
