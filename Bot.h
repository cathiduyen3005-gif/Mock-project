#ifndef Bot_H
#define Bot_H

#include <utility>
#include <vector>
#include "Board.h"

class Bot {
private:
    char aiSymbol;
    char opponentSymbol;
    int difficulty; // 1 = Easy, 2 = Normal, 3 = Hard

    // Hard AI parameters
    static const int MAX_DEPTH = 4;
    
    // Heuristic scoring helpers
    int evaluateBoard(const Board& board) const;
    int evaluateWindow(int aiCount, int opponentCount) const;
    std::vector<std::pair<int, int>> getCandidateMoves(const Board& board) const;
    
    // Alpha-Beta Minimax search
    int minimax(Board& board, int depth, int alpha, int beta, bool isMax, std::pair<int, int>& bestMove);

public:
    Bot(char aiSymbol = 'O', char opponentSymbol = 'X', int difficulty = 2);

    void setDifficulty(int diff);
    int getDifficulty() const { return difficulty; }

    // Computes the best move and returns it as {row, col}
    std::pair<int, int> getBestMove(const Board& board);

    // AI logic variations
    std::pair<int, int> getEasyMove(const Board& board);
    std::pair<int, int> getNormalMove(const Board& board);
    std::pair<int, int> getHardMove(const Board& board);
};

#endif // Bot_H
