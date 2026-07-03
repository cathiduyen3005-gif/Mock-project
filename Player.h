#ifndef PLAYER_H
#define PLAYER_H

#include <string>

class Player {
private:
    std::string name;
    int wins;
    int losses;
    int draws;

public:
    // Constructors
    Player();
    Player(const std::string& name, int wins = 0, int losses = 0, int draws = 0);

    // Getters and Setters
    std::string getName() const;
    int getWins() const;
    int getLosses() const;
    int getDraws() const;

    void addWin();
    void addLoss();
    void addDraw();

    // Helper functions
    double getWinRate() const;
    int getTotalGames() const;
    void displayInfo() const;

    // File serialization helper
    std::string serialize() const;
    static Player deserialize(const std::string& line);
};

#endif // PLAYER_H
