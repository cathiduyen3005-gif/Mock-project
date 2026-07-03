#ifndef BOARD_H
#define BOARD_H

class Board {
public:
    static const int SIZE = 10;

private:
    char grid[SIZE][SIZE];

public:
    Board();

    // Board manipulations
    void clear();
    bool placeMove(int r, int c, char symbol);
    bool isValidMove(int r, int c) const;
    void setCell(int r, int c, char val); // Directly set value (for AI/undo)

    // Getters
    char getCell(int r, int c) const;
    int getSize() const { return SIZE; }

    // Game state checks
    bool checkWin(char symbol) const;
    bool checkDraw() const;

    // Display
    void display() const;
};

#endif // BOARD_H
