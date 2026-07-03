#include <iostream>
#include <string>
#include "PlayerManager.h"
#include "ReplayManager.h"
#include "Game.h"

void displayGuide() {
    std::cout << "\n=============================================\n";
    std::cout << "               CARO GAME GUIDE               \n";
    std::cout << "=============================================\n";
    std::cout << "Rules:\n";
    std::cout << "1. The game is played on a 10x10 grid.\n";
    std::cout << "2. Player 1 is X and Player 2 (or BOT) is O.\n";
    std::cout << "3. Players take turns placing their mark in an empty cell.\n";
    std::cout << "4. To win, a player must get exactly 5 marks in a row,\n";
    std::cout << "   either horizontally, vertically, or diagonally.\n";
    std::cout << "5. If all 100 cells are filled and no one has won, the\n";
    std::cout << "   game is declared a draw.\n\n";
    std::cout << "How to play:\n";
    std::cout << "- When prompted, enter your move coordinates as:\n";
    std::cout << "    [row] [column]\n";
    std::cout << "  (e.g., '0 2' means row 0, column 2).\n";
    std::cout << "- Row and column indexes range from 0 to 9.\n";
    std::cout << "=============================================\n";
    std::cout << "Press Enter to return to main menu...";
    std::string dummy;
    std::getline(std::cin, dummy);
}

void playWithBotMenu(Game& game) {
    while (true) {
        std::cout << "\n*-------Play with BOT-------*\n";
        std::cout << "Press number to choice:\n";
        std::cout << "1.Easy Mode\n";
        std::cout << "2.Normal Mode\n";
        std::cout << "3.Hard Mode\n";
        std::cout << "4.Back to MAIN MENU\n";
        std::cout << "Press number to choice: ";

        std::string choiceStr;
        std::getline(std::cin, choiceStr);
        if (choiceStr.empty()) continue;

        if (choiceStr == "1") {
            game.startPvE(1);
            break;
        } else if (choiceStr == "2") {
            game.startPvE(2);
            break;
        } else if (choiceStr == "3") {
            game.startPvE(3);
            break;
        } else if (choiceStr == "4") {
            break; // Back to main menu
        } else {
            std::cout << "[Error] Invalid option. Please select 1-4.\n";
        }
    }
}

int main() {
    PlayerManager playerManager("players.txt");
    ReplayManager replayManager("replays");
    Game game(playerManager, replayManager);

    while (true) {
        std::cout << "\n*-------MAIN MENU-------*\n";
        std::cout << "Press number to choice:\n";
        std::cout << "1.Play with Other Player\n";
        std::cout << "2.Play with BOT\n";
        std::cout << "3.Replay\n";
        std::cout << "4.Player's Infomation\n"; // Matches 'Infomation' typo from requirement PDF if desired, or 'Information'
        std::cout << "5.Guild\n"; // Matches 'Guild' typo from requirement PDF
        std::cout << "6.Exit\n";
        std::cout << "Press number to choice: ";

        std::string choiceStr;
        std::getline(std::cin, choiceStr);
        if (choiceStr.empty()) continue;

        if (choiceStr == "1") {
            game.startPvP();
        } else if (choiceStr == "2") {
            playWithBotMenu(game);
        } else if (choiceStr == "3") {
            replayManager.startReplaySelection();
        } else if (choiceStr == "4") {
            playerManager.listAllPlayers();
            std::cout << "\nEnter Player Name to Search: ";
            std::string name;
            std::getline(std::cin, name);
            if (!name.empty()) {
                playerManager.searchAndPrintPlayer(name);
            }
            std::cout << "\nPress Enter to return to main menu...";
            std::string dummy;
            std::getline(std::cin, dummy);
        } else if (choiceStr == "5") {
            displayGuide();
        } else if (choiceStr == "6") {
            std::cout << "\nThank you for playing! Exiting...\n";
            break;
        } else {
            std::cout << "[Error] Invalid option. Please select 1-6.\n";
        }
    }

    return 0;
}
