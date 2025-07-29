#include <iostream>
#include <vector>
#include <cctype>
#include <chrono>

#include "Matrix.h"
#include "Gameplay.h"
#include "ConsoleHandler.h"
#include "RNGEngine.h"
#include "FileHandler.h"

using std::cout;
using std::pair;
using std::vector;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::chrono::duration_cast;
std::chrono::high_resolution_clock;

void Gameplay::printMatrixCharacter(char symbol) const {
    if (symbol == 'R') {
        cout << ANSICodes::ROBOT_STYLE << 'R' << ANSICodes::RESET;
    }
    else if (symbol == 'M') {
        cout << ANSICodes::MINOTAUR_STYLE << 'M' << ANSICodes::RESET;
    }
    else if (symbol == 'P') {
        cout << ANSICodes::ITEM_STYLE << 'P' << ANSICodes::RESET;
    }
    else if (symbol == 'E') {
        cout << ANSICodes::EXIT_STYLE << 'E' << ANSICodes::RESET;
    }
    else if (symbol == '#') {
        cout << ANSICodes::WALL_STYLE << '#' << ANSICodes::RESET;
    }
    else if (symbol == 'U') {
		cout << ANSICodes::ENTRANCE_STYLE << 'U' << ANSICodes::RESET;
	}
	else if (symbol == 'I') {
		cout << ANSICodes::EXIT_STYLE << 'I' << ANSICodes::RESET;
	}
    else {
        cout << symbol;
    }
}

void Gameplay::updateMatrixCharacter(unsigned int x, unsigned int y, char symbol) const {
	moveCursorToMatrixPosition(x, y, height, initial_console_size);
	printMatrixCharacter(symbol);
	cout.flush(); 
}

// Position cursor at robot's location so that it blinks there
void Gameplay::positionCursorAtRobot() const {
	moveCursorToMatrixPosition(robot_x, robot_y, height, initial_console_size);
}

void Gameplay::printHermesSpeech() const {
    cout << "\x1B[38;2;0;0;155;47m" << "\n - A swift message from Hermes, messenger of the gods: \n" << ANSICodes::RESET;
    cout << "\n   \"Brave traveler, I guide all who wander through unknown paths.\n";
    cout << "    Use WASD to move your mechanical companion through this labyrinth -\n";
    cout << "    W for north, A for west, S for south, D for east.\n";
    cout << "    If the divine display becomes corrupted, press E to restore it.\n";
    cout << "    Should you wish to return to the mortal realm, press Q to depart.\n\n";
    cout << "    Move wisely, for speed and cunning shall serve you well here.\n";
    cout << "    May the gods favor your journey!\"\n\n";
}

void Gameplay::printHephaestusSpeech() const {
    cout << "\x1B[38;2;0;0;155;47m" << "\n - Hephaestus, god of forge, warns: \n" << ANSICodes::RESET;
    cout << "\n   \"Beware, mortal! I have scattered my crafted relics throughout this maze.\n";
    cout << "    Each 'P' holds a mystery - you won't know what I've forged until you step upon it!\n\n";
    cout << "    My divine creations include:\n";
    cout << "	  * Sword - Sharp enough to cut through even a Minotaur's hide! *forge-ive me the pun*\n";
    cout << "	  * Shield - Defense so strong, it'll make you feel *metal-ly* prepared!\n";
    cout << "	  * Hammer - Breaks walls like my legendary smithing breaks expectations!\n";
    cout << "	  * Fog of War - Clouds your vision... I was having a *mist-ical* day when I made this one!\n\n";
    cout << "    Remember: Each blessing lasts but 3 moves. Use them *smith-ly*!\"\n\n\n";
}

void Gameplay::printWelcomeMessage() const {
    cout << "\n";
    cout << "\x1B[38;2;0;0;155;47m";
    cout << "                                                                           \n";
    cout << " ============= Welcome to the Labyrinth of Knossos, Theseus! ============= \n";
    cout << "                                                                           \n";
    cout << ANSICodes::RESET;
    cout << "\n\n\n";
}

void Gameplay::printDaedalusLegend() const {
    auto duration_microseconds = matrix_generation_time;
    auto duration_milliseconds = duration_cast<milliseconds>(matrix_generation_time);

    cout << "\x1B[38;2;0;0;155;47m" << " - Quick Trivia: " << ANSICodes::RESET << " Legend says that it took Daedalus only " << duration_milliseconds.count() << " ms ("
        << duration_microseconds.count() << " microseconds) to build the labyrinth (apparently Zeus helped him)...\n\n";
}

void Gameplay::initializeGame(unsigned int no_of_items) {
	printWelcomeMessage();

	matrix = new Matrix(width, height);
	matrix_generation_time = matrix->generateMatrix(no_of_items);
	
	robot_x = matrix->getEntranceX();
	robot_y = 1;

	pair<unsigned int, unsigned int> minotaurPosition = matrix->getRandomPassageForMinotaur(robot_x);
	minotaur_x = minotaurPosition.first;
	minotaur_y = minotaurPosition.second;

    printHermesSpeech();
    printHephaestusSpeech();

	matrix->printMatrix(robot_x, robot_y, minotaur_x, minotaur_y);

	initial_console_size = getConsoleSize();
}

pair<unsigned int, unsigned int> Gameplay::getMinotaurBounceCoordinates() {
    // Calculate all possible positions 2 fields away from robot
    vector<pair<unsigned int, unsigned int>> validBouncePositions;

    // Check all positions in a 5x5 grid centered on robot (distance 2 from robot)
    for (int dx = -2; dx <= 2; dx++) {
        for (int dy = -2; dy <= 2; dy++) {
            // Skip positions that are not exactly distance 2
            if (abs(dx) + abs(dy) != 2) continue;

            int new_x = static_cast<int>(robot_x) + dx;
            int new_y = static_cast<int>(robot_y) + dy;

            // Check if position is within bounds
            if (new_x >= 0 && new_x < static_cast<int>(width) &&
                new_y >= 0 && new_y < static_cast<int>(height)) {

                // Check if position is walkable
                if (matrix->getField(static_cast<unsigned int>(new_x),
                    static_cast<unsigned int>(new_y))->isWalkable()) {
                    validBouncePositions.push_back({ static_cast<unsigned int>(new_x),
                                                   static_cast<unsigned int>(new_y) });
                }
            }
        }
    }

    // If we found valid positions, bounce minotaur to one of them
    if (!validBouncePositions.empty()) {
        int randomIndex = RNGEngine::getRandomNumber(0, validBouncePositions.size() - static_cast<unsigned int>(1));
        pair<unsigned int, unsigned int> bouncePosition = validBouncePositions[randomIndex];

        return bouncePosition;
    }
}

bool Gameplay::minotaurAlive() const {
	return minotaur_x != -1 && minotaur_y != -1;
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
        if (sword_rounds_left == 0) {
            new_minotaur_x = robot_x;
            new_minotaur_y = robot_y;

            if (shield_rounds_left > 0) {
				pair<unsigned int, unsigned int> bouncePosition = getMinotaurBounceCoordinates();
				new_minotaur_x = bouncePosition.first;
				new_minotaur_y = bouncePosition.second;
            }
        }
		else {
			// If robot has sword, minotaur dies
			new_minotaur_x = -1;
			new_minotaur_y = -1;
            ariadneCongratulates();
		}
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
            int randomIndex = RNGEngine::getRandomNumber(0, validDirections.size() - static_cast<unsigned int>(1));
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

        // If still alive
		if (minotaurAlive()) {
            // Destroy item he stepped on, if any
            if (matrix->getFieldType(minotaur_x, minotaur_y) == FieldType::ITEM) {
                matrix->setField(minotaur_x, minotaur_y, FieldType::PASSAGE);
            }
            
			// Draw minotaur at new position
            updateMatrixCharacter(minotaur_x, minotaur_y, 'M');
		}
    }
}

bool Gameplay::checkGameEndConditions() {
    auto game_end_time = high_resolution_clock::now();
    auto game_duration = duration_cast<microseconds>(game_end_time - game_start_time);

    // Check if robot reached exit
    if (matrix->getFieldType(robot_x, robot_y) == FieldType::EXIT) {
        fileHandler->saveGameResult(matrix, robot_x, robot_y, minotaur_x, minotaur_y,
            GameResult::VICTORY, game_duration, moves_made);


        moveCursorToMatrixPosition(-3, robot_y + static_cast<unsigned int>(4), height, initial_console_size);
        cout << "\x1B[38;2;255;215;0;46m" << "\n - Zeus, King of Olympus, thunders from above: \n" << ANSICodes::RESET;
        cout << "\n\n   \"MAGNIFICENT, MORTAL! Your courage rivals that of the greatest heroes!\n";
        cout << "    By my lightning bolt, you have conquered the labyrinth that has claimed countless souls!\n\n";
        cout << "    The very stones of Knossos tremble before your triumph!\n";
        cout << "    Even your Father, Poseidon, bows to your superior wit and valor!\n\n";
        cout << "    Let it be known across all realms - from the depths of Hades to the heights of Olympus -\n";
        cout << "    that THIS day, a true champion walked among us!\n\n";
        cout << "\x1B[38;2;255;215;0;46m" << "    ==== THE HEAVENS REJOICE! ====    \n" << ANSICodes::RESET << "\n\n\n";
        
        return true;
    }

    // Check if minotaur caught robot
    if (robot_x == minotaur_x && robot_y == minotaur_y) {
        fileHandler->saveGameResult(matrix, robot_x, robot_y, minotaur_x, minotaur_y,
            GameResult::DEFEATED_BY_MINOTAUR, game_duration, moves_made);

        moveCursorToMatrixPosition(-3, height + static_cast<unsigned int>(2), height, initial_console_size);
        cout << "\x1B[38;2;0;151;255;47m" << "\n - Poseidon, Lord of the Seas, emerges from the depths: \n" << ANSICodes::RESET;
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

void Gameplay::fillEffectHearts(unsigned int y, unsigned int no_of_hearts) {
    moveCursorToMatrixPosition(3 + width + 20, y, height, initial_console_size);

    for (unsigned int i = 0; i < 3; ++i) {
        if (i < no_of_hearts) {
            cout << "\x1B[41;36m" << "0" << ANSICodes::RESET;
        }
        else {
            cout << "\x1B[31m" << "0" << ANSICodes::RESET;
        }
        if (i < 2) {
            cout << " ";
        }
    }

    cout.flush();

    moveCursorToMatrixPosition(3 + width + 3, y, height, initial_console_size);
    if (no_of_hearts > 0) cout << "\x1B[5;36m" << "o" << ANSICodes::RESET;
	else cout << ANSICodes::RESET << "o" << ANSICodes::RESET;
	cout.flush();
}

void Gameplay::recalculateEffects() {
    if (sword_rounds_left > 0) {
        --sword_rounds_left;
        fillEffectHearts(1, sword_rounds_left);
    }
    if (shield_rounds_left > 0) {
        --shield_rounds_left;
        fillEffectHearts(3, shield_rounds_left);
    }
    if (hammer_rounds_left > 0) {
        --hammer_rounds_left;
        fillEffectHearts(5, hammer_rounds_left);
    }
    if (fog_of_war_rounds_left > 0) {
        --fog_of_war_rounds_left;
		if (fog_of_war_rounds_left == 0) {
			redrawMatrixAfterFog();
		}
        fillEffectHearts(7, fog_of_war_rounds_left);
    }
	positionCursorAtRobot();
}

void Gameplay::activateEffect(ItemType itemType) {
    switch (itemType) {
    case ItemType::SWORD:
		sword_rounds_left = 4;
		fillEffectHearts(1, sword_rounds_left);
        break;
    case ItemType::SHIELD:
		shield_rounds_left = 4;
		fillEffectHearts(3, shield_rounds_left);
        break;
    case ItemType::HAMMER:
		hammer_rounds_left = 4;
		fillEffectHearts(5, hammer_rounds_left);
        break;
    case ItemType::FOG_OF_WAR:
		fog_of_war_rounds_left = 4;
		fillEffectHearts(7, fog_of_war_rounds_left);
        break;
    }
}

void Gameplay::ariadneCongratulates() const {
    moveCursorToMatrixPosition(3 + width + 32, 0, height, initial_console_size);
    cout << "\x1B[35;47m" << " - Ariadne, Princess of Crete, emerges from the shadows: " << ANSICodes::RESET;
    moveCursorToMatrixPosition(3 + width + 32, 2, height, initial_console_size);
    cout << "  \"Theseus! My heart soars like a dove freed from its cage!\n";
    moveCursorToMatrixPosition(3 + width + 32, 3, height, initial_console_size);
    cout << "   You have done what no hero before you could accomplish -\n";
    moveCursorToMatrixPosition(3 + width + 32, 4, height, initial_console_size);
    cout << "   you've slain the beast that has haunted my father's kingdom!\n\n";
    moveCursorToMatrixPosition(3 + width + 32, 5, height, initial_console_size);
    cout << "   Here, take this golden thread as a token of my gratitude.\n";
    moveCursorToMatrixPosition(3 + width + 32, 6, height, initial_console_size);
    cout << "   It shall guide you safely to the exit, for I know every\n";
    moveCursorToMatrixPosition(3 + width + 32, 7, height, initial_console_size);
    cout << "   secret passage of this labyrinth by heart.\n\n";
    moveCursorToMatrixPosition(3 + width + 32, 8, height, initial_console_size);
    cout << "   But still... there is something far more precious -\n";
    moveCursorToMatrixPosition(3 + width + 32, 9, height, initial_console_size);
    cout << "   the love of one who has waited long for a true hero!\"\n\n";

    moveCursorToMatrixPosition(3 + width + 32, 11, height, initial_console_size);
    cout << "\x1B[38;2;255;215;0m" << " -----<3----<3----<3----<3----<3----<3----<3----<3\n" << ANSICodes::RESET;
    positionCursorAtRobot();
}

void Gameplay::drawFog() const {
    if (fog_of_war_rounds_left > 0) {
		moveCursorToMatrixPosition(0, 0, height, initial_console_size);
        for (int i = 0; i < height; i++) {
            if (abs((int)i - (int)robot_y) <= 1) {
                for (int j = 0; j < width; j++) {
                    if (abs((int)j - (int)robot_x) > 1) {
                        int rnum = RNGEngine::getRandomNumber(1, 15);
                        char symbol = rnum == 1 ? '#' : ' ';
                        rnum = RNGEngine::getRandomNumber(1, 2);
                        if (rnum % 2 == 0) {
                            cout << "\x1B[5;34;48;5;248m" << symbol << ANSICodes::RESET;
                        }
                        else {
                            cout << "\x1B[5;35;48;5;248m" << symbol << ANSICodes::RESET;
                        }
                    }
					else {
						char symbol = matrix->getField(j, i)->getSymbol();
						if (robot_x == j && robot_y == i) {
							symbol = 'R';
						}
						else if (minotaur_x == j && minotaur_y == i) {
							symbol = 'M';
						}
                        printMatrixCharacter(symbol);
					}
                }
                moveCursorToMatrixPosition(0, i+1, height, initial_console_size);
                continue;
            }
            for (int j = 0; j < width; j++) {
				int rnum = RNGEngine::getRandomNumber(1, 15);
				char symbol = rnum == 1 ? '#' : ' ';
				rnum = RNGEngine::getRandomNumber(1, 2);
                if (rnum % 2 == 0) {
                    cout << "\x1B[5;35;48;5;248m" << symbol << ANSICodes::RESET;
                }
                else {
                    cout << "\x1B[5;34;48;5;248m" << symbol << ANSICodes::RESET;
                }
            }
            cout << "\n  ";
        }
        
        cout.flush();

        positionCursorAtRobot();
    }
}

void Gameplay::redrawMatrixAfterFog() const {
    if (fog_of_war_rounds_left == 0) {
        moveCursorToMatrixPosition(0, 0, height, initial_console_size);
        for (int i = 0; i < height; i++) {
                for (int j = 0; j < width; j++) {
                    char symbol = matrix->getField(j, i)->getSymbol();
                    if (robot_x == j && robot_y == i) {
                        symbol = 'R';
                    }
                    else if (minotaur_x == j && minotaur_y == i) {
                        symbol = 'M';
                    }
                    printMatrixCharacter(symbol);
                }
            cout << "\n  ";
        }

        cout.flush();

        positionCursorAtRobot();
    }
}

void Gameplay::drawBrittleWalls() const {
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            int x = robot_x + dx;
            int y = robot_y + dy;

            if (abs(dx - dy) == 1 &&
                matrix->getFieldType(x, y) == FieldType::WALL &&
                !matrix->isBoundaryOrOutside(x, y)) {

                moveCursorToMatrixPosition(x, y, height, initial_console_size);
                cout << "\x1B[5m#" << ANSICodes::RESET;
				cout.flush();
            }
        }
    }
    positionCursorAtRobot();
}

void Gameplay::redrawWallsNormally(unsigned int prev_robot_x, unsigned int prev_robot_y) const {
    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            int x = prev_robot_x + dx;
            int y = prev_robot_y + dy;

            if (abs(dx - dy) == 1 &&
                matrix->getFieldType(x, y) == FieldType::WALL &&
                !matrix->isBoundaryOrOutside(x, y)) {

                moveCursorToMatrixPosition(x, y, height, initial_console_size);
                cout << ANSICodes::WALL_STYLE << '#' << ANSICodes::RESET;
                cout.flush();
            }
        }
    }
    positionCursorAtRobot();
}

void Gameplay::refreshDisplay() {

    initial_console_size = getConsoleSize();

    clearScreen();

	printWelcomeMessage();
    printDaedalusLegend();
	printHermesSpeech();
	printHephaestusSpeech();

    // Redraw the entire game state
    matrix->printMatrix(robot_x, robot_y, minotaur_x, minotaur_y);

    // Redraw all effect hearts with current values
    fillEffectHearts(1, sword_rounds_left);
    fillEffectHearts(3, shield_rounds_left);
    fillEffectHearts(5, hammer_rounds_left);
    fillEffectHearts(7, fog_of_war_rounds_left);

    // Apply current visual effects if active
    if (fog_of_war_rounds_left > 0) {
        drawFog();
    }
    if (hammer_rounds_left > 0) {
        drawBrittleWalls();
    }

    // Position cursor at robot
    positionCursorAtRobot();
    cout.flush();
}

void Gameplay::startGameLoop() {
    bool gameRunning = true;
	bool gaveUpWithQ = false;

    hideCursor();

    // Store previous positions to clear old characters
    unsigned int prev_robot_x = robot_x;
    unsigned int prev_robot_y = robot_y;
    unsigned int prev_minotaur_x = minotaur_x;
    unsigned int prev_minotaur_y = minotaur_y;

    // Position cursor at robot initially and show it
    positionCursorAtRobot();
	showCursor();

    bool robotMoved = false;

    while (gameRunning) {
		if (robotMoved) recalculateEffects();
		if (robotMoved && fog_of_war_rounds_left > 0) {
            drawFog();
		}
		if (hammer_rounds_left > 0) {
			drawBrittleWalls();
		}

        // Get valid input - this will ONLY return w, a, s, d, or q
        // Invalid keys are silently ignored
        char input = getValidKeyPress();

        // Hide cursor during updates
        cout << "\033[?25l";

        // Handle user input
        robotMoved = false;
        unsigned int new_robot_x = robot_x;
        unsigned int new_robot_y = robot_y;

        switch (input) {
        case 'w':
            if (robot_y > 0 && (matrix->getField(robot_x, robot_y - 1)->isWalkable() || hammer_rounds_left > 0)) {
                new_robot_y = robot_y - 1;
                robotMoved = true;
            }
            break;
        case 's': 
            if (robot_y < height - 1 && (matrix->getField(robot_x, robot_y + 1)->isWalkable() || hammer_rounds_left > 0)) {
                new_robot_y = robot_y + 1;
                robotMoved = true;
            }
            break;
        case 'a': 
            if (robot_x > 0 && (matrix->getField(robot_x - 1, robot_y)->isWalkable() || hammer_rounds_left > 0)) {
                new_robot_x = robot_x - 1;
                robotMoved = true;
            }
            break;
        case 'd': 
            if (robot_x < width - 1 && (matrix->getField(robot_x + 1, robot_y)->isWalkable() || hammer_rounds_left > 0)) {
                new_robot_x = robot_x + 1;
                robotMoved = true;
            }
            break;
		case 'e':
            refreshDisplay();
			continue; 
        case 'q': 
            gameRunning = false;
			gaveUpWithQ = true;

            auto game_end_time = high_resolution_clock::now();
            auto game_duration = duration_cast<microseconds>(game_end_time - game_start_time);

            fileHandler->saveGameResult(matrix, robot_x, robot_y, minotaur_x, minotaur_y,
                GameResult::FORFEITED, game_duration, moves_made);

            continue;
        }

        if (robotMoved) {
            moves_made++;

            // Clear robot's old position (restore underlying field symbol)
            MatrixField* oldField = matrix->getField(prev_robot_x, prev_robot_y);
            updateMatrixCharacter(prev_robot_x, prev_robot_y, oldField->getSymbol());

            // Update robot position
            robot_x = new_robot_x;
            robot_y = new_robot_y;

			// Check if robot stepped on an item
			if (matrix->getFieldType(robot_x, robot_y) == FieldType::ITEM) {
                MatrixField* field = matrix->getField(robot_x, robot_y);
                Item* item = dynamic_cast<Item*>(field);

                if (item != nullptr) {
                    activateEffect(item->getItemType());
					matrix->setField(robot_x, robot_y, FieldType::PASSAGE);
                }
			}

			// If robot stepped on a brittle wall, destroy it
			if (hammer_rounds_left > 0 && matrix->getFieldType(robot_x, robot_y) == FieldType::WALL) {
				matrix->setField(robot_x, robot_y, FieldType::PASSAGE);
			}

            // Draw robot at new position
            updateMatrixCharacter(robot_x, robot_y, 'R');

            if (hammer_rounds_left > 0) {
                redrawWallsNormally(prev_robot_x, prev_robot_y);
            }

            // Store current positions as previous for next iteration
            prev_robot_x = robot_x;
            prev_robot_y = robot_y;

            if (minotaurAlive()) {
                // Now handle Minotaur movement
                moveMinotaur(prev_minotaur_x, prev_minotaur_y);
                prev_minotaur_x = minotaur_x;
                prev_minotaur_y = minotaur_y;
            }

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
		moveCursorToMatrixPosition(-3, height + static_cast<unsigned int>(2), height, initial_console_size);
        cout << "\x1B[35;47m" << "\n - Athena, Goddess of Wisdom and Strategy, appears: \n" << ANSICodes::RESET;
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