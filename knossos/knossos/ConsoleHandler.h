#pragma once

using std::pair;

char getValidKeyPress();

pair<int, int> getConsoleSize();

void moveCursorToMatrixPosition(unsigned int x, unsigned int y, unsigned int height, pair<int, int> initial_console_size);
