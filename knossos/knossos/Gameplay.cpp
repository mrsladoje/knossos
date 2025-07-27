#include <iostream>
#include <vector>
#include <cctype>

#ifdef _WIN32
    #include <conio.h>      // Windows: for _getch()
#else
    #include <termios.h>    // Unix/Linux/Mac: for terminal control
    #include <unistd.h>     // Unix/Linux/Mac: for STDIN_FILENO
#endif

#include "Matrix.h"
#include "Gameplay.h"

using std::cout;
using std::cin;
using std::tolower;
using std::pair;
using std::make_pair;
using std::vector;


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

void Gameplay::moveCursorToMatrixPosition(unsigned int x, unsigned int y) const {
	int console_width = initial_console_size.first;
	int console_height = initial_console_size.second;

    // Calculate matrix start position from bottom
    unsigned int matrix_start_from_bottom = height + 1; 
    unsigned int terminal_row = console_height - matrix_start_from_bottom + y;
    unsigned int terminal_col = x + 3; 

    cout << "\033[" << terminal_row << ";" << terminal_col << "H";
}

void Gameplay::updateMatrixCharacter(unsigned int x, unsigned int y, char symbol) const {
	moveCursorToMatrixPosition(x, y);
	if (symbol == 'R') {
        cout << "\x1B[5;34;48;2;110;110;110m" << 'R' << "\x1B[0m";
	}
	else if (symbol == 'M') {
        cout << "\x1B[5;38;2;150;75;0;48;2;110;110;110m" << 'M' << "\x1B[0m";
	}
	else if (symbol == 'P') {
        cout << "\x1B[31m" << 'P' << "\x1B[0m"; 
	}
	else if (symbol == 'E') {
		cout << "\x1B[5;32m" << 'E' << "\x1B[0m"; 
	}
	else {
		cout << symbol; 
	}
	cout.flush(); 
}

// Position cursor at robot's location so that it blinks there
void Gameplay::positionCursorAtRobot() const {
	moveCursorToMatrixPosition(robot_x, robot_y);
}

void Gameplay::initializeGame(unsigned int no_of_items) {

	cout << "\n";
	cout << "\x1B[38;2;0;0;155;47m";
	cout << "                                                                           \n";
	cout << " ============= Welcome to the Labyrinth of Knossos, Theseus! ============= \n";
	cout << "                                                                           \n";
	cout << "\x1B[0m";
	cout << "\n\n\n";

	matrix = new Matrix(width, height);
	matrix->generateMatrix(no_of_items);
	
	robot_x = matrix->getEntranceX();
	robot_y = 1;

	pair<unsigned int, unsigned int> minotaurPosition = matrix->getRandomPassageForMinotaur(robot_x);
	minotaur_x = minotaurPosition.first;
	minotaur_y = minotaurPosition.second;

	cout << "\x1B[38;2;0;0;155;47m" << "\n - A swift message from Hermes, messenger of the gods: \n" << "\x1B[0m";
	cout << "\n   \"Brave traveler, I guide all who wander through unknown paths.\n";
	cout << "    Use WASD to move your mechanical companion through this labyrinth -\n";
	cout << "    W for north, A for west, S for south, D for east.\n";
    cout << "    Should you wish to return to the mortal realm, press Q to depart.\n\n";
	cout << "    Move wisely, for speed and cunning shall serve you well here.\n";
	cout << "    May the gods favor your journey!\"\n\n";

	cout << "\x1B[38;2;0;0;155;47m" << "\n - Hephaestus, god of forge, warns: \n" << "\x1B[0m";
	cout << "\n   \"Beware, mortal! I have scattered my crafted relics throughout this maze.\n";
	cout << "    Each 'P' holds a mystery - you won't know what I've forged until you step upon it!\n\n";
	cout << "    My divine creations include:\n";
	cout << "	  * Sword - Sharp enough to cut through even a Minotaur's hide! *forge-ive me the pun*\n";
	cout << "	  * Shield - Defense so strong, it'll make you feel *metal-ly* prepared!\n";
	cout << "	  * Hammer - Breaks walls like my legendary smithing breaks expectations!\n";
	cout << "	  * Fog of War - Clouds your vision... I was having a *mist-ical* day when I made this one!\n\n";
	cout << "    Remember: Each blessing lasts but 3 moves. Use them *smith-ly*!\"\n\n\n";

	matrix->printMatrix(robot_x, robot_y, minotaur_x, minotaur_y);

	initial_console_size = getConsoleSize();
}

void Gameplay::moveMinotaur(unsigned int prev_minotaur_x, unsigned int prev_minotaur_y) {
    unsigned int new_minotaur_x = minotaur_x;
    unsigned int new_minotaur_y = minotaur_y;

    // Check if robot is adjacent to minotaur (eating range)
    bool robotInEatingRange = false;
    if ((abs((int)robot_x - (int)minotaur_x) <= 1) && (abs((int)robot_y - (int)minotaur_y) <= 1) &&
        !(robot_x == minotaur_x && robot_y == minotaur_y)) {

        // Check if there's a direct path to robot (no walls between)
        if ((robot_x == minotaur_x && abs((int)robot_y - (int)minotaur_y) == 1) ||
            (robot_y == minotaur_y && abs((int)robot_x - (int)minotaur_x) == 1)) {
            robotInEatingRange = true;
        }
    }

    // MUST move toward robot to eat it
    if (robotInEatingRange) {
        new_minotaur_x = robot_x;
        new_minotaur_y = robot_y;
    }
    else {
        vector<int> validDirections;

        if (minotaur_y > 0 && matrix->getField(minotaur_x, minotaur_y - 1)->isWalkable()) {
            validDirections.push_back(0);
        }
        if (minotaur_y < height - 1 && matrix->getField(minotaur_x, minotaur_y + 1)->isWalkable()) {
            validDirections.push_back(1);
        }
        if (minotaur_x > 0 && matrix->getField(minotaur_x - 1, minotaur_y)->isWalkable()) {
            validDirections.push_back(2);
        }
        if (minotaur_x < width - 1 && matrix->getField(minotaur_x + 1, minotaur_y)->isWalkable()) {
            validDirections.push_back(3);
        }

        if (!validDirections.empty()) {
            int randomIndex = matrix->getRandomNumber(0, validDirections.size() - 1);
            int direction = validDirections[randomIndex];

            switch (direction) {
            case 0:
                new_minotaur_y = minotaur_y - 1;
                break;
            case 1: 
                new_minotaur_y = minotaur_y + 1;
                break;
            case 2: 
                new_minotaur_x = minotaur_x - 1;
                break;
            case 3: 
                new_minotaur_x = minotaur_x + 1;
                break;
            }
        }
    }

    // Only update if minotaur actually moved
    if (new_minotaur_x != minotaur_x || new_minotaur_y != minotaur_y) {
        // Clear minotaur's old position
        MatrixField* oldField = matrix->getField(prev_minotaur_x, prev_minotaur_y);
        updateMatrixCharacter(prev_minotaur_x, prev_minotaur_y, oldField->getSymbol());

        // Update minotaur position
        minotaur_x = new_minotaur_x;
        minotaur_y = new_minotaur_y;

		// Destroy item he stepped on, if any
		if (matrix->getFieldType(minotaur_x, minotaur_y) == FieldType::ITEM) {
			matrix->setField(minotaur_x, minotaur_y, FieldType::PASSAGE);
		}

        // Draw minotaur at new position    
        updateMatrixCharacter(minotaur_x, minotaur_y, 'M');
    }
}

bool Gameplay::checkGameEndConditions() {
    // Check if robot reached exit
    if (matrix->getFieldType(robot_x, robot_y) == FieldType::EXIT) {
        moveCursorToMatrixPosition(-3, robot_y + static_cast<unsigned int>(4));
        cout << "\x1B[38;2;255;215;0;46m" << "\n - Zeus, King of Olympus, thunders from above: \n" << "\x1B[0m";
        cout << "\n\n   \"MAGNIFICENT, MORTAL! Your courage rivals that of the greatest heroes!\n";
        cout << "    By my lightning bolt, you have conquered the labyrinth that has claimed countless souls!\n\n";
        cout << "    The very stones of Knossos tremble before your triumph!\n";
        cout << "    Even your Father, Poseidon, bows to your superior wit and valor!\n\n";
        cout << "    Let it be known across all realms - from the depths of Hades to the heights of Olympus -\n";
        cout << "    that THIS day, a true champion walked among us!\n\n";
        cout << "\x1B[38;2;255;215;0;46m" << "    ==== THE HEAVENS REJOICE! ====    \n" << "\x1B[0m\n\n\n";
        return true;
    }

    // Check if minotaur caught robot
    if (robot_x == minotaur_x && robot_y == minotaur_y) {
        moveCursorToMatrixPosition(-3, height + static_cast<unsigned int>(2));
        cout << "\x1B[38;2;0;151;255;47m" << "\n - Poseidon, Lord of the Seas, emerges from the depths: \n" << "\x1B[0m";
        cout << "\n\n   \"My son... my brave Theseus...\n";
        cout << "    I have watched your journey through these cursed halls with great pride.\n\n";
        cout << "    Though your mechanical companion has fallen, your courage burns brighter\n";
        cout << "    than the fire of Olympus itself!\n\n";
        cout << "    Do not let this defeat *tide* you over with despair - I shall craft you\n";
        cout << "    a new ally from the depths of my ocean forge!\n\n";
        cout << "    Rise again, my child. The sea never yields to any beast!\"\n\n\n";
        return true;
    }

    return false;
}

char Gameplay::getValidKeyPress() {
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

void Gameplay::startGameLoop() {
    bool gameRunning = true;
	bool gaveUpWithQ = false;

    // Hide cursor during initial setup
    cout << "\033[?25l";

    // Store previous positions to clear old characters
    unsigned int prev_robot_x = robot_x;
    unsigned int prev_robot_y = robot_y;
    unsigned int prev_minotaur_x = minotaur_x;
    unsigned int prev_minotaur_y = minotaur_y;

    // Position cursor at robot initially and show it
    positionCursorAtRobot();
    cout << "\033[?25h"; // Show cursor

    while (gameRunning) {
        // Get valid input - this will ONLY return w, a, s, d, or q
        // Invalid keys are silently ignored
        char input = getValidKeyPress();

        // Hide cursor during updates
        cout << "\033[?25l";

        // Handle user input
        bool robotMoved = false;
        unsigned int new_robot_x = robot_x;
        unsigned int new_robot_y = robot_y;

        switch (input) {
        case 'w':
            if (robot_y > 0 && matrix->getField(robot_x, robot_y - 1)->isWalkable()) {
                new_robot_y = robot_y - 1;
                robotMoved = true;
            }
            break;
        case 's': 
            if (robot_y < height - 1 && matrix->getField(robot_x, robot_y + 1)->isWalkable()) {
                new_robot_y = robot_y + 1;
                robotMoved = true;
            }
            break;
        case 'a': 
            if (robot_x > 0 && matrix->getField(robot_x - 1, robot_y)->isWalkable()) {
                new_robot_x = robot_x - 1;
                robotMoved = true;
            }
            break;
        case 'd': 
            if (robot_x < width - 1 && matrix->getField(robot_x + 1, robot_y)->isWalkable()) {
                new_robot_x = robot_x + 1;
                robotMoved = true;
            }
            break;
        case 'q': 
            gameRunning = false;
			gaveUpWithQ = true;
            continue;
        }

        if (robotMoved) {
            // Clear robot's old position (restore underlying field symbol)
            MatrixField* oldField = matrix->getField(prev_robot_x, prev_robot_y);
            updateMatrixCharacter(prev_robot_x, prev_robot_y, oldField->getSymbol());

            // Update robot position
            robot_x = new_robot_x;
            robot_y = new_robot_y;

            // Draw robot at new position
            updateMatrixCharacter(robot_x, robot_y, 'R');

            // Store current positions as previous for next iteration
            prev_robot_x = robot_x;
            prev_robot_y = robot_y;

            // Now handle Minotaur movement
            moveMinotaur(prev_minotaur_x, prev_minotaur_y);
            prev_minotaur_x = minotaur_x;
            prev_minotaur_y = minotaur_y;

            // Check for game end conditions
            if (checkGameEndConditions()) {
                gameRunning = false;
                continue;
            }
        }

        // Position cursor at robot and show it for next input
        positionCursorAtRobot();
        cout << "\033[?25h";
    }

	if (gaveUpWithQ) {
		moveCursorToMatrixPosition(-3, height + static_cast<unsigned int>(2));
        cout << "\x1B[35;47m" << "\n - Athena, Goddess of Wisdom and Strategy, appears: \n" << "\x1B[0m";
        cout << "\n\n   \"Hold, brave Theseus! Do not let frustration cloud your judgment!\n";
        cout << "    Even the wisest warriors must sometimes retreat to fight another day.\n\n";
        cout << "    This labyrinth requires more than courage - it demands cunning and strategy.\n";
        cout << "    Perhaps... you should seek counsel from one who knows these halls intimately.\n\n";
        cout << "    There is a maiden named Ariadne, daughter of King Minos himself.\n";
        cout << "    Her knowledge of this maze surpasses even my own divine wisdom!\n\n";
        cout << "    Look for her gentle hands - they hold the key to your escape,\n";
        cout << "    and perhaps something more precious than mere survival.\n\n";
        cout << "    Trust in her guidance, Theseus, for love and courage together\n";
        cout << "    can unravel even the most impossible of tangles!\"\n\n\n";
	}
}