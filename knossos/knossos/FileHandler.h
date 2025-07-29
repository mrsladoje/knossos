#pragma once

#include <string>
#include <chrono>

// Forward declaration
class Matrix;

enum class GameResult {
    VICTORY,               // Player reached the exit
    MINOTAUR_SLAIN,        // Player killed the minotaur with sword
    DEFEATED_BY_MINOTAUR,  // Player was caught by minotaur
    FORFEITED              // Player quit with 'Q'
};

class FileHandler {
private:
    std::string generateTimestamp() const;
    std::string gameResultToString(GameResult result) const;
    std::string generateFilename() const;

public:
    FileHandler() = default;
    ~FileHandler() = default;

    // Main method to save game state and result
    bool saveGameResult(const Matrix* matrix,
        unsigned int robot_x, unsigned int robot_y,
        unsigned int minotaur_x, unsigned int minotaur_y,
        GameResult result,
        const std::chrono::microseconds& game_duration,
        unsigned int moves_made) const;

    // Helper method to save just the matrix state (useful for debugging)
    bool saveMatrixState(const Matrix* matrix,
        unsigned int robot_x, unsigned int robot_y,
        unsigned int minotaur_x, unsigned int minotaur_y,
        const std::string& filename) const;
};
