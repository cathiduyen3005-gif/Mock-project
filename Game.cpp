#include "Game.h"
#include <iostream>
#include <sstream>
#include <thread>
#include <chrono>

Game::Game(PlayerManager& pm, ReplayManager& rm) 
    : playerManager(pm), replayManager(rm) {}

bool Game::getCoordinates(int& r, int& c, const std::string& playerName) {
    while (true) {
        std::cout << playerName << "'s turn: ";
        std::string inputLine;
        if (!std::getline(std::cin, inputLine)) {
            return false;
        }

        if (inputLine.empty()) continue;

        std::stringstream ss(inputLine);
        if (ss >> r >> c) {
            if (board.isValidMove(r, c)) {
                return true;
            } else {
                std::cout << "[Error] Invalid coordinates or cell already occupied. Try again (Row and Col between 0 and 9).\n";
            }
        } else {
            std::cout << "[Error] Invalid input format. Enter two numbers separated by a space (e.g., '0 2').\n";
        }
    }
}

void Game::printWinnerHUD(const std::string& winnerName, char symbol) const {
    std::cout << "\n=============================================\n";
    std::cout << "  VICTORY! " << winnerName << " (" << symbol << ") wins!\n";
    std::cout << "=============================================\n";
}

void Game::printDrawHUD() const {
    std::cout << "\n=============================================\n";
    std::cout << "  DRAW! The board is full.\n";
    std::cout << "=============================================\n";
}

void Game::startPvP() {
    std::string p1Name, p2Name;
    std::cout << "\n*------- PLAY WITH OTHER PLAYER -------*\n";
    
    // Player 1
    std::cout << "Enter Player 1 Name (X): ";
    std::getline(std::cin, p1Name);
    if (p1Name.empty()) p1Name = "Player1";
    Player* p1 = playerManager.getOrCreatePlayer(p1Name);

    // Player 2
    std::cout << "Enter Player 2 Name (O): ";
    std::getline(std::cin, p2Name);
    if (p2Name.empty()) p2Name = "Player2";
    Player* p2 = playerManager.getOrCreatePlayer(p2Name);

    // Initialize board and move logs
    board.clear();
    moveHistory.clear();

    bool isP1Turn = true;
    bool gameEnded = false;

    std::cout << "\n>>> Starting game: " << p1->getName() << " vs " << p2->getName() << " <<<\n";
    std::cout << "=> Player 1 = X, Player 2 = O:\n";

    board.display();

    while (!gameEnded) {
        int r = -1, c = -1;
        std::string currentName = isP1Turn ? p1->getName() : p2->getName();
        char currentSymbol = isP1Turn ? 'X' : 'O';

        if (!getCoordinates(r, c, currentName)) {
            std::cout << "\nGame aborted.\n";
            return;
        }

        // Bước di chuyển
        board.placeMove(r, c, currentSymbol);
        moveHistory.push_back({r, c});

        // Vẽ lại bàn cờ sau mỗi bước di chuyển
        std::cout << "\nPlayer " << (isP1Turn ? "1" : "2") << " enters " << r << " " << c 
                  << ", saves the moves and prints the new checker status:\n";
        std::cout << "=> Player 1 = X, Player 2 = O:\n";
        board.display();

        // Kiểm tra trạng thái thắng/thua/hòa sau mỗi bước di chuyển
        if (board.checkWin(currentSymbol)) {
            printWinnerHUD(currentName, currentSymbol);
            if (isP1Turn) {
                p1->addWin();
                p2->addLoss();
            } else {
                p2->addWin();
                p1->addLoss();
            }
            gameEnded = true;
        } else if (board.checkDraw()) {
            printDrawHUD();
            p1->addDraw();
            p2->addDraw();
            gameEnded = true;
        } else {
            isP1Turn = !isP1Turn;
        }
    }

    // Lưu và cập nhật thông tin người chơi, lưu lại replay
    playerManager.saveToFile();
    replayManager.saveReplay("PvP", p1->getName(), p2->getName(), moveHistory);

    std::cout << "\nPress Enter to return to main menu...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

void Game::startPvE(int botDifficulty) {
    std::string pName;
    std::string diffName = (botDifficulty == 1) ? "Easy" : (botDifficulty == 2) ? "Normal" : "Hard";
    
    std::cout << "\n*------- PLAY WITH BOT (" << diffName << ") -------*\n";
    std::cout << "Enter Your Name (X): ";
    std::getline(std::cin, pName);
    if (pName.empty()) pName = "Player";
    Player* p = playerManager.getOrCreatePlayer(pName);

    std::string botName = "BOT_" + diffName;

    // Đối tượng Bot với độ khó đã chọn
    Bot bot('O', 'X', botDifficulty);

    // Dọn bàn cờ và lịch sử di chuyển trước khi bắt đầu
    board.clear();
    moveHistory.clear();

    bool isHumanTurn = true;
    bool gameEnded = false;

    std::cout << "\n>>> Starting game: " << p->getName() << " vs " << botName << " <<<\n";
    std::cout << "=> Player 1 = X, Player 2 = O:\n";

    board.display();

    while (!gameEnded) {
        int r = -1, c = -1;

        if (isHumanTurn) {
            if (!getCoordinates(r, c, p->getName())) {
                std::cout << "\nGame aborted.\n";
                return;
            }
            board.placeMove(r, c, 'X');
            moveHistory.push_back({r, c});

            std::cout << "\nPlayer 1 enters " << r << " " << c 
                      << ", saves the moves and prints the new checker status:\n";
            std::cout << "=> Player 1 = X, Player 2 = O:\n";
            board.display();

            if (board.checkWin('X')) {
                printWinnerHUD(p->getName(), 'X');
                p->addWin();
                gameEnded = true;
            } else if (board.checkDraw()) {
                printDrawHUD();
                p->addDraw();
                gameEnded = true;
            } else {
                isHumanTurn = false;
            }
        } else {
            std::cout << botName << "'s turn: ";
            std::cout.flush();

            // Mô phỏng thời gian suy nghĩ của BOT
            std::this_thread::sleep_for(std::chrono::milliseconds(500));

            std::pair<int, int> botMove = bot.getBestMove(board);
            r = botMove.first;
            c = botMove.second;

            // Bot trả về nước đi
            std::cout << r << " " << c << "\n";

            board.placeMove(r, c, 'O');
            moveHistory.push_back({r, c});

            std::cout << "\nBOT enters " << r << " " << c 
                      << ", saves the moves and prints the new checker status:\n";
            std::cout << "=> Player 1 = X, Player 2 = O:\n";
            board.display();

            if (board.checkWin('O')) {
                printWinnerHUD(botName, 'O');
                p->addLoss();
                gameEnded = true;
            } else if (board.checkDraw()) {
                printDrawHUD();
                p->addDraw();
                gameEnded = true;
            } else {
                isHumanTurn = true;
            }
        }
    }

    // Lưu và cập nhật thông tin người chơi, lưu lại replay
    playerManager.saveToFile();
    replayManager.saveReplay("PvE_" + diffName, p->getName(), botName, moveHistory);

    std::cout << "\nPress Enter to return to main menu...";
    std::string dummy;
    std::getline(std::cin, dummy);
}
