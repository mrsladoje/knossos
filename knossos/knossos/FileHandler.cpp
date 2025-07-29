#include "FileHandler.h"
#include "Matrix.h"
#include "MatrixField.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <ctime>

using std::ofstream;
using std::string;
using std::to_string;
using std::cerr;
using std::endl;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::duration_cast;

string FileHandler::generateTimestamp() const {
    auto now = std::time(nullptr);

    struct tm timeinfo;
#ifdef _WIN32
    localtime_s(&timeinfo, &now);
#else
    timeinfo = *std::localtime(&now);
#endif

    std::ostringstream oss;
    oss << std::put_time(&timeinfo, "%Y%m%d_%H%M%S");
    return oss.str();
}

string FileHandler::gameResultToString(GameResult result) const {
    switch (result) {
    case GameResult::VICTORY:
        return "VICTORY - Reached the exit successfully!";
    case GameResult::MINOTAUR_SLAIN:
        return "MINOTAUR_SLAIN - Defeated the minotaur with sword!";
    case GameResult::DEFEATED_BY_MINOTAUR:
        return "DEFEATED - Caught by the minotaur!";
    case GameResult::FORFEITED:
        return "FORFEITED - Player quit the game";
    default:
        return "UNKNOWN";
    }
}

string FileHandler::generateFilename() const {
    return "labyrinth_game_" + generateTimestamp() + ".txt";
}

bool FileHandler::saveGameResult(const Matrix* matrix,
    unsigned int robot_x, unsigned int robot_y,
    unsigned int minotaur_x, unsigned int minotaur_y,
    GameResult result,
    const microseconds& game_duration,
    unsigned int moves_made) const {

    if (!matrix) {
        cerr << "Error: Matrix pointer is null!" << endl;
        return false;
    }

    string filename = generateFilename();
    ofstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Could not create file " << filename << endl;
        return false;
    }

    // Write header with game information
    file << "=====================================" << endl;
    file << "   LABYRINTH OF KNOSSOS - GAME RESULT" << endl;
    file << "=====================================" << endl;
    file << "Game completed at: " << generateTimestamp() << endl;
    file << "Final result: " << gameResultToString(result) << endl;
    file << "Game duration: " << duration_cast<milliseconds>(game_duration).count()
        << " ms (" << game_duration.count() << " microseconds)" << endl;
    file << "Total moves made: " << moves_made << endl;
    file << "=====================================" << endl << endl;

    // Write final positions
    file << "FINAL POSITIONS:" << endl;
    file << "Robot (R): (" << robot_x << ", " << robot_y << ")" << endl;

    if (minotaur_x == static_cast<unsigned int>(-1) && minotaur_y == static_cast<unsigned int>(-1)) {
        file << "Minotaur (M): SLAIN" << endl;
    }
    else {
        file << "Minotaur (M): (" << minotaur_x << ", " << minotaur_y << ")" << endl;
    }
    file << endl;

    // Write legend
    file << "LEGEND:" << endl;
    file << "R = Robot (Theseus)" << endl;
    file << "M = Minotaur" << endl;
    file << "U = Entrance" << endl;
    file << "E = Exit" << endl;
    file << "I = Exit (alternate symbol)" << endl;
    file << "P = Item (Sword/Shield/Hammer/Fog of War)" << endl;
    file << "# = Wall" << endl;
    file << "  = Passage" << endl;
    file << endl;

    // Write final matrix state
    file << "FINAL MATRIX STATE:" << endl;
    file << endl;

    // Get matrix dimensions (assuming these methods exist or can be added)
    unsigned int width = matrix->getWidth();
    unsigned int height = matrix->getHeight();

    // Add column numbers header
    file << "   ";
    for (unsigned int j = 0; j < width; ++j) {
        file << (j % 10);
    }
    file << endl;

    // Print matrix with row numbers
    for (unsigned int i = 0; i < height; ++i) {
        file << std::setw(2) << i << " ";

        for (unsigned int j = 0; j < width; ++j) {
            char symbol;

            // Check for robot position
            if (robot_x == j && robot_y == i) {
                symbol = 'R';
            }
            // Check for minotaur position (if still alive)
            else if (minotaur_x == j && minotaur_y == i &&
                !(minotaur_x == static_cast<unsigned int>(-1) &&
                    minotaur_y == static_cast<unsigned int>(-1))) {
                symbol = 'M';
            }
            // Get the field symbol
            else {
                MatrixField* field = matrix->getField(j, i);
                symbol = field ? field->getSymbol() : '#';
            }

            file << symbol;
        }
        file << endl;
    }

    file << endl;
    file << "=====================================" << endl;
    file << "Game saved successfully!" << endl;
    file << "=====================================" << endl;

    file.close();

    return true;
}

bool FileHandler::saveMatrixState(const Matrix* matrix,
    unsigned int robot_x, unsigned int robot_y,
    unsigned int minotaur_x, unsigned int minotaur_y,
    const string& filename) const {

    if (!matrix) {
        cerr << "Error: Matrix pointer is null!" << endl;
        return false;
    }

    ofstream file(filename);

    if (!file.is_open()) {
        cerr << "Error: Could not create file " << filename << endl;
        return false;
    }

    file << "MATRIX STATE SNAPSHOT" << endl;
    file << "=====================" << endl;
    file << "Timestamp: " << generateTimestamp() << endl;
    file << "Robot position: (" << robot_x << ", " << robot_y << ")" << endl;
    file << "Minotaur position: (" << minotaur_x << ", " << minotaur_y << ")" << endl;
    file << endl;

    unsigned int width = matrix->getWidth();
    unsigned int height = matrix->getHeight();

    for (unsigned int i = 0; i < height; ++i) {
        for (unsigned int j = 0; j < width; ++j) {
            if (robot_x == j && robot_y == i) {
                file << 'R';
            }
            else if (minotaur_x == j && minotaur_y == i) {
                file << 'M';
            }
            else {
                MatrixField* field = matrix->getField(j, i);
                file << (field ? field->getSymbol() : '#');
            }
        }
        file << endl;
    }

    file.close();
    return true;
}