#include "Player.h"
#include <iostream>
#include <sstream>
#include <iomanip>

Player::Player() : name(""), wins(0), losses(0), draws(0) {}

Player::Player(const std::string& name, int wins, int losses, int draws)
    : name(name), wins(wins), losses(losses), draws(draws) {}

std::string Player::getName() const {
    return name;
}

int Player::getWins() const {
    return wins;
}

int Player::getLosses() const {
    return losses;
}

int Player::getDraws() const {
    return draws;
}

void Player::addWin() {
    wins++;
}

void Player::addLoss() {
    losses++;
}

void Player::addDraw() {
    draws++;
}

double Player::getWinRate() const {
    int total = getTotalGames();
    if (total == 0) return 0.0;
    return (static_cast<double>(wins) / total) * 100.0;
}

int Player::getTotalGames() const {
    return wins + losses + draws;
}

void Player::displayInfo() const {
    std::cout << "-------------------------------------------\n";
    std::cout << " Player Profile: " << name << "\n";
    std::cout << "-------------------------------------------\n";
    std::cout << "  Wins:   " << wins << "\n";
    std::cout << "  Losses: " << losses << "\n";
    std::cout << "  Draws:  " << draws << "\n";
    std::cout << "  Total:  " << getTotalGames() << "\n";
    std::cout << "  Win Rate: " << std::fixed << std::setprecision(1) << getWinRate() << "%\n";
    std::cout << "-------------------------------------------\n";
}

std::string Player::serialize() const {
    // Format: name,wins,losses,draws
    // To support names with spaces safely, we'll replace spaces with underscores during file save if needed,
    // but a comma-separated format is clean.
    return name + "," + std::to_string(wins) + "," + std::to_string(losses) + "," + std::to_string(draws);
}

Player Player::deserialize(const std::string& line) {
    std::stringstream ss(line);
    std::string nameToken, winsStr, lossesStr, drawsStr;

    std::getline(ss, nameToken, ',');
    std::getline(ss, winsStr, ',');
    std::getline(ss, lossesStr, ',');
    std::getline(ss, drawsStr, ',');

    int w = winsStr.empty() ? 0 : std::stoi(winsStr);
    int l = lossesStr.empty() ? 0 : std::stoi(lossesStr);
    int d = drawsStr.empty() ? 0 : std::stoi(drawsStr);

    return Player(nameToken, w, l, d);
}
