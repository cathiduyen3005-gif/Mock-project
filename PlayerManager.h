#ifndef PLAYERMANAGER_H
#define PLAYERMANAGER_H

#include <vector>
#include <string>
#include "Player.h"

class PlayerManager {
private:
    std::vector<Player> players;
    std::string filePath;

public:
    PlayerManager(const std::string& filePath = "players.txt");

    // File operation functions
    void loadFromFile();
    void saveToFile();

    // Player operations
    Player* getOrCreatePlayer(const std::string& name);
    Player* getPlayer(const std::string& name);
    
    // Search and matching operations (Requirement R4)
    void searchAndPrintPlayer(const std::string& name) const;
    const Player* findEquivalentPlayer(const Player& targetPlayer) const;

    // Utility
    void listAllPlayers() const;
};

#endif // PLAYERMANAGER_H
