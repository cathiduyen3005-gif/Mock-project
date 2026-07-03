#include "ReplayManager.h"
#include "Board.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <chrono>
#include <iomanip>
#include <algorithm>

namespace fs = std::filesystem;

ReplayManager::ReplayManager(const std::string& folderPath) : folderPath(folderPath) {
    createDirectoryIfNotExists();
}

bool ReplayManager::createDirectoryIfNotExists() {
    try {
        if (!fs::exists(folderPath)) {
            return fs::create_directories(folderPath);
        }
        return true;
    } catch (...) {
        return false;
    }
}

std::vector<std::string> ReplayManager::getReplayFiles() const {
    std::vector<std::string> files;
    if (!fs::exists(folderPath)) return files;

    try {
        for (const auto& entry : fs::directory_iterator(folderPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".txt") {
                files.push_back(entry.path().filename().string());
            }
        }
    } catch (...) {
        // Safe check
    }

    // Sort files so newest (by name timestamp) appear first or alphabetically
    std::sort(files.begin(), files.end(), std::greater<std::string>());
    return files;
}

bool ReplayManager::saveReplay(const std::string& mode, const std::string& p1, const std::string& p2, 
                              const std::vector<std::pair<int, int>>& moves) {
    if (!createDirectoryIfNotExists()) {
        std::cerr << "[Error] Could not access or create replays directory." << std::endl;
        return false;
    }

    // Generate unique timestamp filename
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << folderPath << "/replay_" << std::put_time(std::localtime(&in_time_t), "%Y%m%d_%H%M%S") << ".txt";
    std::string fileName = ss.str();

    std::ofstream file(fileName);
    if (!file.is_open()) {
        std::cerr << "[Error] Failed to open file to save replay: " << fileName << std::endl;
        return false;
    }

    // Format:
    // Line 1: Mode
    // Line 2: Player 1 Name
    // Line 3: Player 2 Name
    // Line 4: r,c;r,c;r,c;...
    file << mode << "\n";
    file << p1 << "\n";
    file << p2 << "\n";

    for (size_t i = 0; i < moves.size(); ++i) {
        file << moves[i].first << "," << moves[i].second;
        if (i < moves.size() - 1) {
            file << ";";
        }
    }
    file << "\n";
    file.close();
    return true;
}

void ReplayManager::startReplaySelection() {
    std::vector<std::string> files = getReplayFiles();
    if (files.empty()) {
        std::cout << "\n[Info] No saved replay logs found. Play some games first!\n";
        std::cout << "Press Enter to return to main menu...";
        std::string dummy;
        std::getline(std::cin, dummy);
        return;
    }

    while (true) {
        std::cout << "\n=============================================\n";
        std::cout << "               GAME REPLAYS LIST             \n";
        std::cout << "=============================================\n";
        
        // Show details of each file
        struct ReplayHeader {
            std::string fileName;
            std::string mode;
            std::string p1;
            std::string p2;
        };
        std::vector<ReplayHeader> headers;

        for (size_t i = 0; i < files.size(); ++i) {
            std::ifstream file(folderPath + "/" + files[i]);
            if (file.is_open()) {
                std::string mode, p1, p2;
                std::getline(file, mode);
                std::getline(file, p1);
                std::getline(file, p2);
                headers.push_back({files[i], mode, p1, p2});
                
                std::cout << " " << (i + 1) << ". " << files[i] << "\n"
                          << "    [" << mode << "] " << p1 << " vs " << p2 << "\n";
            }
        }
        std::cout << " " << (files.size() + 1) << ". Back to MAIN MENU\n";
        std::cout << "---------------------------------------------\n";
        std::cout << "Select a game to replay (1-" << (files.size() + 1) << "): ";

        std::string choiceStr;
        std::getline(std::cin, choiceStr);
        int choice = 0;
        try {
            choice = std::stoi(choiceStr);
        } catch (...) {
            std::cout << "[Error] Invalid input. Please enter a number.\n";
            continue;
        }

        if (choice == static_cast<int>(files.size() + 1)) {
            break; // Back to main menu
        }

        if (choice < 1 || choice > static_cast<int>(files.size())) {
            std::cout << "[Error] Choice out of range.\n";
            continue;
        }

        // Play the chosen replay
        playReplay(files[choice - 1]);
    }
}

void ReplayManager::playReplay(const std::string& fileName) {
    std::ifstream file(folderPath + "/" + fileName);
    if (!file.is_open()) {
        std::cout << "[Error] Could not load replay file.\n";
        return;
    }

    std::string mode, p1, p2, movesStr;
    std::getline(file, mode);
    std::getline(file, p1);
    std::getline(file, p2);
    std::getline(file, movesStr);
    file.close();

    // Parse moves
    std::vector<std::pair<int, int>> moves;
    if (!movesStr.empty()) {
        std::stringstream ss(movesStr);
        std::string moveToken;
        while (std::getline(ss, moveToken, ';')) {
            if (moveToken.empty()) continue;
            std::stringstream moveSS(moveToken);
            std::string rStr, cStr;
            if (std::getline(moveSS, rStr, ',') && std::getline(moveSS, cStr, ',')) {
                try {
                    moves.push_back({std::stoi(rStr), std::stoi(cStr)});
                } catch (...) {}
            }
        }
    }

    if (moves.empty()) {
        std::cout << "[Info] This replay contains no moves.\n";
        return;
    }

    // Step through replay
    int currentMoveIndex = 0; // 0 = empty board, 1 = first move placed, etc.
    int totalMoves = static_cast<int>(moves.size());

    while (true) {
        // Reconstruct board state up to currentMoveIndex
        Board board;
        for (int i = 0; i < currentMoveIndex; ++i) {
            char symbol = (i % 2 == 0) ? 'X' : 'O';
            board.placeMove(moves[i].first, moves[i].second, symbol);
        }

        // Draw HUD
        std::cout << "\n=============================================\n";
        std::cout << " REPLAYING: " << p1 << " (X) vs " << p2 << " (O)\n";
        std::cout << " Mode:      " << mode << "\n";
        std::cout << " Move:      " << currentMoveIndex << " / " << totalMoves << "\n";
        std::cout << "=============================================\n";

        board.display();

        if (currentMoveIndex > 0) {
            int lastIdx = currentMoveIndex - 1;
            std::string playerWhoMoved = (lastIdx % 2 == 0) ? p1 : p2;
            char sym = (lastIdx % 2 == 0) ? 'X' : 'O';
            std::cout << ">>> Move " << currentMoveIndex << ": " 
                      << playerWhoMoved << " (" << sym << ") placed at (" 
                      << moves[lastIdx].first << ", " << moves[lastIdx].second << ")\n";
        } else {
            std::cout << ">>> Game starting board (empty)\n";
        }

        std::cout << "\n[Controls]: [Enter] Next Move | [b] Step Back | [q] Exit Replay\n";
        std::cout << "Choice: ";
        
        std::string command;
        std::getline(std::cin, command);
        
        // Normalize command to lowercase
        std::transform(command.begin(), command.end(), command.begin(), ::tolower);

        if (command == "q") {
            break;
        } else if (command == "b") {
            if (currentMoveIndex > 0) {
                currentMoveIndex--;
            } else {
                std::cout << "[Info] Already at the starting board.\n";
            }
        } else {
            // Default: advance move
            if (currentMoveIndex < totalMoves) {
                currentMoveIndex++;
            } else {
                std::cout << "[Info] Reached the end of the game replay.\n";
                // Let user check victory
                bool p1Wins = board.checkWin('X');
                bool p2Wins = board.checkWin('O');
                if (p1Wins) {
                    std::cout << ">>> Result: " << p1 << " (X) won the game!\n";
                } else if (p2Wins) {
                    std::cout << ">>> Result: " << p2 << " (O) won the game!\n";
                } else {
                    std::cout << ">>> Result: Match ended in a draw!\n";
                }
                std::cout << "Press [q] to exit, or [b] to step back: ";
                std::getline(std::cin, command);
                if (command == "q" || command == "Q") {
                    break;
                } else if (command == "b" || command == "B") {
                    currentMoveIndex--;
                }
            }
        }
    }
}
