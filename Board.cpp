#include "Board.h"
#include <iostream>

Board::Board() {
    clear();
}

void Board::clear() {
    for (int i = 0; i < SIZE; ++i) {
        for (int j = 0; j < SIZE; ++j) {
            grid[i][j] = ' ';
        }
    }
}

bool Board::placeMove(int r, int c, char symbol) {
    if (isValidMove(r, c)) {
        grid[r][c] = symbol;
        return true;
    }
    return false;
}

bool Board::isValidMove(int r, int c) const {
    return (r >= 0 && r < SIZE && c >= 0 && c < SIZE && grid[r][c] == ' ');
}

void Board::setCell(int r, int c, char val) {
    if (r >= 0 && r < SIZE && c >= 0 && c < SIZE) {
        grid[r][c] = val;
    }
}

char Board::getCell(int r, int c) const {
    if (r >= 0 && r < SIZE && c >= 0 && c < SIZE) {
        return grid[r][c];
    }
    return ' ';
}

bool Board::checkWin(char symbol) const {
    // Check all cells as starting point for a 5-in-a-row
    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            if (grid[r][c] != symbol) continue;

            // 1. Horizontal (right)
            if (c + 4 < SIZE) {
                if (grid[r][c+1] == symbol && grid[r][c+2] == symbol &&
                    grid[r][c+3] == symbol && grid[r][c+4] == symbol) {
                    return true;
                }
            }

            // 2. Vertical (down)
            if (r + 4 < SIZE) {
                if (grid[r+1][c] == symbol && grid[r+2][c] == symbol &&
                    grid[r+3][c] == symbol && grid[r+4][c] == symbol) {
                    return true;
                }
            }

            // 3. Diagonal Down-Right
            if (r + 4 < SIZE && c + 4 < SIZE) {
                if (grid[r+1][c+1] == symbol && grid[r+2][c+2] == symbol &&
                    grid[r+3][c+3] == symbol && grid[r+4][c+4] == symbol) {
                    return true;
                }
            }

            // 4. Diagonal Up-Right
            if (r - 4 >= 0 && c + 4 < SIZE) {
                if (grid[r-1][c+1] == symbol && grid[r-2][c+2] == symbol &&
                    grid[r-3][c+3] == symbol && grid[r-4][c+4] == symbol) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool Board::checkDraw() const {
    for (int r = 0; r < SIZE; ++r) {
        for (int c = 0; c < SIZE; ++c) {
            if (grid[r][c] == ' ') {
                return false; // Found an empty cell, not a draw
            }
        }
    }
    return true; // No empty cells left
}

void Board::display() const {
    std::cout << "\n";
    std::cout << "     0   1   2   3   4   5   6   7   8   9\n";
    std::cout << "   |---|---|---|---|---|---|---|---|---|---|\n";
    for (int r = 0; r < SIZE; ++r) {
        std::cout << " " << r << " |";
        for (int c = 0; c < SIZE; ++c) {
            std::cout << " " << grid[r][c] << " |";
        }
        std::cout << "\n";
        std::cout << "   |---|---|---|---|---|---|---|---|---|---|\n";
    }
    std::cout << std::endl;
}
