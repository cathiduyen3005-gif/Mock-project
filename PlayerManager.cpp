#include "PlayerManager.h"
#include <iostream>
#include <fstream>
#include <cmath>
#include <algorithm>
#include <iomanip>

PlayerManager::PlayerManager(const std::string& filePath) : filePath(filePath) {
    loadFromFile();
}

void PlayerManager::loadFromFile() {
    players.clear();
    std::ifstream file(filePath);
    if (!file.is_open()) {
        // File doesn't exist yet, which is fine
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        try {
            players.push_back(Player::deserialize(line));
        } catch (...) {
            // Ignore corrupted lines
        }
    }
    file.close();
}

void PlayerManager::saveToFile() {
    std::ofstream file(filePath);
    if (!file.is_open()) {
        std::cerr << "[Error] Could not save players database to " << filePath << std::endl;
        return;
    }

    for (const auto& player : players) {
        file << player.serialize() << "\n";
    }
    file.close();
}

Player* PlayerManager::getOrCreatePlayer(const std::string& name) {
    // Search for existing player
    Player* p = getPlayer(name);
    if (p != nullptr) {
        return p;
    }

    // Create new player if not found
    players.push_back(Player(name));
    saveToFile(); // Persist immediately
    return &players.back();
}

Player* PlayerManager::getPlayer(const std::string& name) {
    for (auto& player : players) {
        // Case insensitive or exact match? Exact match is safer, but let's do a case-insensitive check
        // for better UX, or standard exact check. Let's do exact check.
        if (player.getName() == name) {
            return &player;
        }
    }
    return nullptr;
}

const Player* PlayerManager::findEquivalentPlayer(const Player& targetPlayer) const {
    if (players.size() <= 1) {
        return nullptr; // No other players to match
    }

    const Player* closestOpponent = nullptr;
    double minDiff = 101.0; // Max difference is 100.0%

    for (const auto& p : players) {
        if (p.getName() == targetPlayer.getName()) {
            continue; // Don't match with self
        }

        double diff = std::abs(targetPlayer.getWinRate() - p.getWinRate());
        if (diff < minDiff) {
            minDiff = diff;
            closestOpponent = &p;
        } else if (diff == minDiff) {
            // In case of a tie, choose the one with closer total games
            if (closestOpponent == nullptr || 
                std::abs(targetPlayer.getTotalGames() - p.getTotalGames()) < 
                std::abs(targetPlayer.getTotalGames() - closestOpponent->getTotalGames())) {
                closestOpponent = &p;
            }
        }
    }

    return closestOpponent;
}

void PlayerManager::searchAndPrintPlayer(const std::string& name) const {
    // Find player in database
    const Player* target = nullptr;
    for (const auto& p : players) {
        if (p.getName() == name) {
            target = &p;
            break;
        }
    }

    if (target == nullptr) {
        std::cout << "\n[Info] Player \"" << name << "\" not found in the records.\n";
        return;
    }

    // Print player profile
    target->displayInfo();

    // Find and print equivalent player (Requirement R4)
    const Player* eqPlayer = findEquivalentPlayer(*target);
    if (eqPlayer != nullptr) {
        std::cout << "\n>>> Equivalent Opponent Match (Closest Level) <<<\n";
        std::cout << "  Name:       " << eqPlayer->getName() << "\n";
        std::cout << "  Win Rate:   " << std::fixed << std::setprecision(1) << eqPlayer->getWinRate() << "%\n";
        std::cout << "  Wins/Losses:" << eqPlayer->getWins() << "W - " << eqPlayer->getLosses() << "L - " << eqPlayer->getDraws() << "D\n";
        std::cout << "  Win Rate Diff: " << std::fixed << std::setprecision(1) 
                  << std::abs(target->getWinRate() - eqPlayer->getWinRate()) << "%\n";
        std::cout << "-------------------------------------------\n";
    } else {
        std::cout << "\n[Info] No other players found to perform level matching.\n";
    }
}

void PlayerManager::listAllPlayers() const {
    if (players.empty()) {
        std::cout << "No players registered yet.\n";
        return;
    }

    std::cout << "\n=============================================\n";
    std::cout << "            REGISTERED PLAYERS LIST           \n";
    std::cout << "=============================================\n";
    std::cout << std::left << std::setw(15) << "Name" 
              << std::right << std::setw(8) << "Wins" 
              << std::setw(8) << "Losses" 
              << std::setw(8) << "Draws" 
              << std::setw(12) << "Win Rate\n";
    std::cout << "---------------------------------------------\n";
    for (const auto& p : players) {
        std::cout << std::left << std::setw(15) << p.getName()
                  << std::right << std::setw(8) << p.getWins()
                  << std::setw(8) << p.getLosses()
                  << std::setw(8) << p.getDraws()
                  << std::setw(11) << std::fixed << std::setprecision(1) << p.getWinRate() << "%\n";
    }
    std::cout << "=============================================\n";
}
