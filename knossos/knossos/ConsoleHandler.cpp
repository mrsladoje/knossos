#include <cctype>       // for tolower
#include <iostream>     

#ifdef _WIN32
#include <conio.h>      // Windows: for _getch()
#else
#include <termios.h>    // Unix/Linux/Mac: for terminal control
#include <unistd.h>     // Unix/Linux/Mac: for STDIN_FILENO
#endif

#include "ConsoleHandler.h"

using std::tolower;
using std::pair;
using std::make_pair;
using std::cout;

#ifdef _WIN32
#include <windows.h>

pair<int, int> getConsoleSize() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    int height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    int width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    return make_pair(width, height);
}
#else
#include <sys/ioctl.h>

pair<int, int> getConsoleSize() {
    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
    return make_pair(w.ws_col, w.ws_row);
}
#endif

char getValidKeyPress() {
    char key;

    while (true) {
#ifdef _WIN32
        // WINDOWS IMPLEMENTATION
        // _getch() reads a single character immediately without Enter
        // It doesn't echo the character to the screen
        key = _getch();

        // Handle special cases on Windows
        if (key == 0 || key == -32) {
            // Arrow keys and function keys send two characters
            // First is 0 or -32, second is the actual key code
            // We read and discard the second character
            _getch();
            continue; // Ignore these keys and wait for another
        }

#else
        // UNIX/LINUX/MAC IMPLEMENTATION
        // We need to temporarily change terminal settings
        struct termios oldTermios, newTermios;

        // Get current terminal settings
        tcgetattr(STDIN_FILENO, &oldTermios);

        // Copy current settings to modify them
        newTermios = oldTermios;

        // Disable canonical mode (line buffering) and echo
        // ICANON: canonical input (wait for Enter)
        // ECHO: echo typed characters to screen
        newTermios.c_lflag &= ~(ICANON | ECHO);

        // Apply new settings immediately
        tcsetattr(STDIN_FILENO, TCSANOW, &newTermios);

        // Read single character
        key = getchar();

        // Restore original terminal settings immediately
        tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);

        // Handle arrow keys on Unix (they send escape sequences)
        if (key == 27) { // ESC character starts arrow key sequence
            // Try to read the next characters of the sequence
            // Set terminal to non-blocking mode temporarily
            struct termios tempTermios = oldTermios;
            tempTermios.c_lflag &= ~(ICANON | ECHO);
            tempTermios.c_cc[VMIN] = 0;  // Don't wait for characters
            tempTermios.c_cc[VTIME] = 1; // Wait 0.1 seconds max
            tcsetattr(STDIN_FILENO, TCSANOW, &tempTermios);

            char seq1 = getchar();
            char seq2 = getchar();

            // Restore settings
            tcsetattr(STDIN_FILENO, TCSANOW, &oldTermios);

            // Ignore arrow keys and continue waiting for valid input
            continue;
        }
#endif

        // Convert to lowercase for consistent handling
        key = tolower(key);

        // Only accept our valid game keys
        if (key == 'w' || key == 'a' || key == 's' || key == 'd' || key == 'q') {
            return key;
        }
    }
}

void moveCursorToMatrixPosition(unsigned int x, unsigned int y, unsigned int height, pair<int, int> initial_console_size) {
    int console_width = initial_console_size.first;
    int console_height = initial_console_size.second;

    // Calculate matrix start position from bottom
    unsigned int matrix_start_from_bottom = height + 1;
    unsigned int terminal_row = console_height - matrix_start_from_bottom + y;
    unsigned int terminal_col = x + 3;

    cout << "\033[" << terminal_row << ";" << terminal_col << "H";
}