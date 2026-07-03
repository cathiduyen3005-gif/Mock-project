#ifndef REPLAYMANAGER_H
#define REPLAYMANAGER_H

#include <vector>
#include <string>
#include <utility>

struct ReplayData {
    std::string mode;
    std::string player1;
    std::string player2;
    std::vector<std::pair<int, int>> moves;
};

class ReplayManager {
private:
    std::string folderPath;

    // Helper functions
    bool createDirectoryIfNotExists();
    std::vector<std::string> getReplayFiles() const;

public:
    ReplayManager(const std::string& folderPath = "replays");

    // Save game replay to file
    bool saveReplay(const std::string& mode, const std::string& p1, const std::string& p2, 
                    const std::vector<std::pair<int, int>>& moves);

    // Replay gameplay step-by-step
    void startReplaySelection();
    void playReplay(const std::string& fileName);
};

#endif // REPLAYMANAGER_H
