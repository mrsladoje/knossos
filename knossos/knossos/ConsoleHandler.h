#pragma once

using std::pair;

namespace ANSICodes {
    extern const char* ROBOT_STYLE;
    extern const char* MINOTAUR_STYLE;
    extern const char* WALL_STYLE;
    extern const char* ENTRANCE_STYLE;
    extern const char* EXIT_STYLE;
    extern const char* ITEM_STYLE;
    extern const char* RESET;
}

char getValidKeyPress();

pair<int, int> getConsoleSize();

void moveCursorToMatrixPosition(unsigned int x, unsigned int y, unsigned int height, pair<int, int> initial_console_size);

void clearScreen();
void hideCursor();
void showCursor();
