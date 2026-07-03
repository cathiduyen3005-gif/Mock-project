#include "Bot.h"
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <climits>
#include <iostream>

Bot::Bot(char aiSymbol, char opponentSymbol, int difficulty)
    : aiSymbol(aiSymbol), opponentSymbol(opponentSymbol), difficulty(difficulty) {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

void Bot::setDifficulty(int diff) {
    if (diff >= 1 && diff <= 3) {
        difficulty = diff;
    }
}

std::pair<int, int> Bot::getBestMove(const Board& board) {
    if (difficulty == 1) {
        return getEasyMove(board);
    } else if (difficulty == 2) {
        return getNormalMove(board);
    } else {
        return getHardMove(board);
    }
}

std::pair<int, int> Bot::getEasyMove(const Board& board) {
    std::vector<std::pair<int, int>> emptyCells;
    int size = board.getSize();
    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            if (board.getCell(r, c) == ' ') {
                emptyCells.push_back({r, c});
            }
        }
    }

    if (emptyCells.empty()) return {-1, -1};
    int idx = std::rand() % emptyCells.size();
    return emptyCells[idx];
}

std::pair<int, int> Bot::getNormalMove(const Board& board) {
    int size = board.getSize();

    // 1. Can AI win in one move?
    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            if (board.getCell(r, c) == ' ') {
                Board tempBoard = board;
                tempBoard.placeMove(r, c, aiSymbol);
                if (tempBoard.checkWin(aiSymbol)) {
                    return {r, c};
                }
            }
        }
    }

    // 2. Can opponent win in one move? Block it.
    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            if (board.getCell(r, c) == ' ') {
                Board tempBoard = board;
                tempBoard.placeMove(r, c, opponentSymbol);
                if (tempBoard.checkWin(opponentSymbol)) {
                    return {r, c};
                }
            }
        }
    }

    // 3. Otherwise, pick the candidate move with the best immediate score
    std::vector<std::pair<int, int>> candidates = getCandidateMoves(board);
    if (candidates.empty()) {
        return getEasyMove(board);
    }

    std::pair<int, int> bestMove = candidates[0];
    int bestScore = -INT_MAX;

    for (const auto& move : candidates) {
        Board tempBoard = board;
        tempBoard.placeMove(move.first, move.second, aiSymbol);
        int score = evaluateBoard(tempBoard);
        if (score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
    }

    return bestMove;
}

std::pair<int, int> Bot::getHardMove(const Board& board) {
    // If board is empty, play in center area
    bool empty = true;
    int size = board.getSize();
    for (int r = 0; r < size && empty; ++r) {
        for (int c = 0; c < size && empty; ++c) {
            if (board.getCell(r, c) != ' ') {
                empty = false;
            }
        }
    }

    if (empty) {
        return {size / 2, size / 2};
    }

    Board tempBoard = board;
    std::pair<int, int> bestMove = {-1, -1};
    minimax(tempBoard, MAX_DEPTH, -INT_MAX, INT_MAX, true, bestMove);

    // Safety fallback
    if (bestMove.first == -1 || !board.isValidMove(bestMove.first, bestMove.second)) {
        return getNormalMove(board);
    }

    return bestMove;
}

int Bot::evaluateBoard(const Board& board) const {
    int score = 0;
    int size = board.getSize();

    // Check all windows of length 5 on the board
    // 1. Horizontal
    for (int r = 0; r < size; ++r) {
        for (int c = 0; c <= size - 5; ++c) {
            int aiCount = 0, oppCount = 0;
            for (int i = 0; i < 5; ++i) {
                char val = board.getCell(r, c + i);
                if (val == aiSymbol) aiCount++;
                else if (val == opponentSymbol) oppCount++;
            }
            score += evaluateWindow(aiCount, oppCount);
        }
    }

    // 2. Vertical
    for (int c = 0; c < size; ++c) {
        for (int r = 0; r <= size - 5; ++r) {
            int aiCount = 0, oppCount = 0;
            for (int i = 0; i < 5; ++i) {
                char val = board.getCell(r + i, c);
                if (val == aiSymbol) aiCount++;
                else if (val == opponentSymbol) oppCount++;
            }
            score += evaluateWindow(aiCount, oppCount);
        }
    }

    // 3. Diagonal Down-Right
    for (int r = 0; r <= size - 5; ++r) {
        for (int c = 0; c <= size - 5; ++c) {
            int aiCount = 0, oppCount = 0;
            for (int i = 0; i < 5; ++i) {
                char val = board.getCell(r + i, c + i);
                if (val == aiSymbol) aiCount++;
                else if (val == opponentSymbol) oppCount++;
            }
            score += evaluateWindow(aiCount, oppCount);
        }
    }

    // 4. Diagonal Up-Right
    for (int r = 4; r < size; ++r) {
        for (int c = 0; c <= size - 5; ++c) {
            int aiCount = 0, oppCount = 0;
            for (int i = 0; i < 5; ++i) {
                char val = board.getCell(r - i, c + i);
                if (val == aiSymbol) aiCount++;
                else if (val == opponentSymbol) oppCount++;
            }
            score += evaluateWindow(aiCount, oppCount);
        }
    }

    return score;
}

int Bot::evaluateWindow(int aiCount, int opponentCount) const {
    if (aiCount > 0 && opponentCount > 0) {
        return 0; // Both symbols exist in window, cannot win here
    }

    if (aiCount == 5) return 10000000;
    if (opponentCount == 5) return -10000000;

    if (aiCount > 0) {
        switch (aiCount) {
            case 4: return 100000;
            case 3: return 1000;
            case 2: return 100;
            case 1: return 10;
        }
    } else if (opponentCount > 0) {
        switch (opponentCount) {
            case 4: return -80000; // slightly lower than AI win, but high priority to block
            case 3: return -800;
            case 2: return -80;
            case 1: return -8;
        }
    }

    return 0;
}

std::vector<std::pair<int, int>> Bot::getCandidateMoves(const Board& board) const {
    std::vector<std::pair<int, int>> candidates;
    int size = board.getSize();

    // To optimize performance, only search empty cells that are close (dist <= 2) to already placed stones.
    // In addition, we score them immediately and sort them.
    struct ScoredMove {
        int r, c;
        int score;
        bool operator<(const ScoredMove& other) const {
            return score > other.score; // Descending order
        }
    };

    std::vector<ScoredMove> scoredMoves;

    for (int r = 0; r < size; ++r) {
        for (int c = 0; c < size; ++c) {
            if (board.getCell(r, c) != ' ') continue;

            // Check if there is any piece nearby
            bool nearPiece = false;
            for (int dr = -2; dr <= 2 && !nearPiece; ++dr) {
                for (int dc = -2; dc <= 2; ++dc) {
                    int nr = r + dr;
                    int nc = c + dc;
                    if (nr >= 0 && nr < size && nc >= 0 && nc < size) {
                        if (board.getCell(nr, nc) != ' ') {
                            nearPiece = true;
                            break;
                        }
                    }
                }
            }

            if (nearPiece) {
                // Heuristic score for this move
                // Sum of AI score and Opponent score at this cell
                int moveScore = 0;
                
                // Let's compute a simple local score around (r,c) to order moves
                for (int dr = -1; dr <= 1; ++dr) {
                    for (int dc = -1; dc <= 1; ++dc) {
                        if (dr == 0 && dc == 0) continue;
                        int nr = r + dr;
                        int nc = c + dc;
                        if (nr >= 0 && nr < size && nc >= 0 && nc < size) {
                            char cellVal = board.getCell(nr, nc);
                            if (cellVal == aiSymbol) moveScore += 10;
                            else if (cellVal == opponentSymbol) moveScore += 8;
                        }
                    }
                }
                scoredMoves.push_back({r, c, moveScore});
            }
        }
    }

    std::sort(scoredMoves.begin(), scoredMoves.end());

    // Take top candidate moves (limit branching factor to top 10)
    int count = 0;
    for (const auto& sm : scoredMoves) {
        candidates.push_back({sm.r, sm.c});
        count++;
        if (count >= 12) break; // Limit candidates for search speed
    }

    // Fallback if no moves are near any pieces (e.g. at the start of game)
    if (candidates.empty()) {
        int r = size / 2;
        int c = size / 2;
        if (board.getCell(r, c) == ' ') {
            candidates.push_back({r, c});
        } else {
            // Find any empty cell
            for (int row = 0; row < size; ++row) {
                for (int col = 0; col < size; ++col) {
                    if (board.getCell(row, col) == ' ') {
                        candidates.push_back({row, col});
                        return candidates;
                    }
                }
            }
        }
    }

    return candidates;
}

int Bot::minimax(Board& board, int depth, int alpha, int beta, bool isMax, std::pair<int, int>& bestMove) {
    // 1. Terminal states
    if (board.checkWin(aiSymbol)) {
        return 10000000 - (MAX_DEPTH - depth); // prefer quicker wins
    }
    if (board.checkWin(opponentSymbol)) {
        return -10000000 + (MAX_DEPTH - depth); // prefer delaying loss
    }
    if (board.checkDraw() || depth == 0) {
        return evaluateBoard(board);
    }

    std::vector<std::pair<int, int>> candidates = getCandidateMoves(board);

    if (isMax) {
        int maxEval = -INT_MAX;
        std::pair<int, int> localBestMove = {-1, -1};
        
        for (const auto& move : candidates) {
            board.setCell(move.first, move.second, aiSymbol);
            std::pair<int, int> dummy;
            int eval = minimax(board, depth - 1, alpha, beta, false, dummy);
            board.setCell(move.first, move.second, ' '); // Undo move

            if (eval > maxEval) {
                maxEval = eval;
                localBestMove = move;
            }
            alpha = std::max(alpha, eval);
            if (beta <= alpha) {
                break; // Beta prune
            }
        }
        bestMove = localBestMove;
        return maxEval;
    } else {
        int minEval = INT_MAX;
        std::pair<int, int> localBestMove = {-1, -1};

        for (const auto& move : candidates) {
            board.setCell(move.first, move.second, opponentSymbol);
            std::pair<int, int> dummy;
            int eval = minimax(board, depth - 1, alpha, beta, true, dummy);
            board.setCell(move.first, move.second, ' '); // Undo move

            if (eval < minEval) {
                minEval = eval;
                localBestMove = move;
            }
            beta = std::min(beta, eval);
            if (beta <= alpha) {
                break; // Alpha prune
            }
        }
        bestMove = localBestMove;
        return minEval;
    }
}
