#include <iostream>
#include <set>
#include <chrono>
#include <stdexcept>

#include "Matrix.h"
#include "MatrixField.h"
#include "ConsoleHandler.h"
#include "RNGEngine.h"

using std::vector;
using std::set;
using std::pair;
using std::make_pair;
using std::cout;
using std::cerr;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::min;
using std::random_shuffle;
using std::out_of_range;

Matrix::Matrix(unsigned int w, unsigned int h)
	: width(w), height(h), fields(nullptr) {

	fields = new MatrixField * *[width];

	for (unsigned int i = 0; i < width; ++i) {
		fields[i] = new MatrixField * [height];
		for (unsigned int j = 0; j < height; ++j) {
			fields[i][j] = new Wall();
		}
	}
}

Matrix::~Matrix() {
	for (unsigned int i = 0; i < width; ++i) {
		for (unsigned int j = 0; j < height; ++j) {
			delete fields[i][j];
		}
		delete[] fields[i];
	}
	delete[] fields;
}

MatrixField* Matrix::getField(unsigned int x, unsigned int y) const {
	if (x < width && y < height) {
		return fields[x][y];
	}
	return nullptr;
}

FieldType Matrix::getFieldType(unsigned int x, unsigned int y) const {
	if (x < width && y < height) {
		return fields[x][y]->getFieldType();
	}
	else {
		return FieldType::WALL;
	}
}

void Matrix::setField(unsigned int x, unsigned int y, FieldType fieldType) {
	if (x < width && x >= 0 && y < height && y >= 0) {
		delete fields[x][y];
		switch (fieldType) {
		case FieldType::PASSAGE:
			fields[x][y] = new Passage();
			break;
		case FieldType::WALL:
			fields[x][y] = new Wall();
			break;
		case FieldType::ENTRANCE:
			fields[x][y] = new Entrance();
			break;
		case FieldType::EXIT:
			fields[x][y] = new Exit();
			break;
		case FieldType::ITEM:
			fields[x][y] = createRandomItem();
			break;
		default:
			fields[x][y] = new Passage();
		}
	}
	else {
		throw out_of_range("Coordinates out of bounds");
	}
}

bool Matrix::isBoundaryOrOutside(unsigned int x, unsigned int y) const {
	return (x <= 0 || x >= width - 1 || y <= 0 || y >= height - 1);
}

pair<unsigned int, unsigned int> Matrix::setEntranceAndExit() {
	unsigned int entrance_x = RNGEngine::getRandomNumber(1, width - 2);
	unsigned int exit_x = RNGEngine::getRandomNumber(1, width - 2);

	delete fields[entrance_x][0];
	fields[entrance_x][0] = new Entrance();

	delete fields[exit_x][height - 1];
	fields[exit_x][height - 1] = new Exit();

	return make_pair(entrance_x, exit_x);
}

void Matrix::generativePrim(unsigned int entrance_x) {

	// first, we clear the robot's starting position, and use it as the seed for generative prim
	delete fields[entrance_x][1];
	fields[entrance_x][1] = new Passage();

	// frontiers are potential extensions to the ever-growing explorable area of the maze
	vector<pair<unsigned int, unsigned int>> frontiers;
	pair<unsigned int, unsigned int> current(entrance_x, 1);
	set<pair<unsigned int, unsigned int>> visited;

	frontiers.push_back(make_pair(entrance_x, 3));
	visited.insert(make_pair(entrance_x, 3));
	if (!isBoundaryOrOutside(entrance_x + 2, 1)) {
		frontiers.push_back(make_pair(entrance_x + 2, 1));
		visited.insert(make_pair(entrance_x + 2, 1));
	}
	if (!isBoundaryOrOutside(entrance_x - 2, 1)) {
		frontiers.push_back(make_pair(entrance_x - 2, 1));
		visited.insert(make_pair(entrance_x - 2, 1));
	}

	while (!frontiers.empty())
	{
		unsigned int chosenOne = RNGEngine::getRandomNumber(0, (unsigned int)frontiers.size() - 1);
		current = frontiers[chosenOne];
		frontiers.erase(frontiers.begin() + chosenOne);
		delete fields[current.first][current.second];
		fields[current.first][current.second] = new Passage();

		vector<pair<unsigned int, unsigned int>> potentialMazeReconnectionPoints;
		potentialMazeReconnectionPoints.push_back(make_pair(current.first, current.second - 2));
		potentialMazeReconnectionPoints.push_back(make_pair(current.first + 2, current.second));
		potentialMazeReconnectionPoints.push_back(make_pair(current.first, current.second + 2));
		potentialMazeReconnectionPoints.push_back(make_pair(current.first - 2, current.second));

		for (auto it = potentialMazeReconnectionPoints.begin(); it != potentialMazeReconnectionPoints.end();) {
			if (isBoundaryOrOutside(it->first, it->second) || !(getFieldType(it->first, it->second) == FieldType::PASSAGE)) {
				if (!isBoundaryOrOutside(it->first, it->second) && visited.find(*it) == visited.end()) {
					frontiers.push_back(make_pair(it->first, it->second));
					visited.insert(make_pair(it->first, it->second));
				}
				it = potentialMazeReconnectionPoints.erase(it);
			}
			else {
				++it;
			}
		}

		pair<unsigned int, unsigned int> chosenPoint = potentialMazeReconnectionPoints[RNGEngine::getRandomNumber(0, (unsigned int)potentialMazeReconnectionPoints.size() - 1)];

		delete fields[(current.first + chosenPoint.first) / 2][(current.second + chosenPoint.second) / 2];
		fields[(current.first + chosenPoint.first) / 2][(current.second + chosenPoint.second) / 2] = new Passage();

	}
}

void Matrix::assurePathConnectivity(unsigned int exit_x) {
	if (height % 2 == 0) {
		delete fields[exit_x][height - 2];
		fields[exit_x][height - 2] = new Passage();

		bool fullPathExists = getFieldType(exit_x, height - 3) == FieldType::PASSAGE;

		// connect to nearest passage in the second-to-last row from the boundary
		if (!fullPathExists) {
			int connectionFound = -1;

			for (int offset = 1; offset < (int)width; ++offset) {
				if (exit_x + offset < width - 1) {
					if (getFieldType(exit_x + offset, height - 3) == FieldType::PASSAGE) {
						connectionFound = exit_x + offset;
						break;
					}
				}
				if (exit_x >= offset && exit_x - offset > 0) {
					if (getFieldType(exit_x - offset, height - 3) == FieldType::PASSAGE) {
						connectionFound = exit_x - offset;
						break;
					}
				}
			}

			if (connectionFound != -1) {
				int start = (exit_x < connectionFound) ? exit_x : connectionFound;
				int end = (exit_x > connectionFound) ? exit_x : connectionFound;

				for (int x = start; x <= end; ++x) {
					if (x > 0 && x < width - 1) {
						delete fields[x][height - 3];
						fields[x][height - 3] = new Passage();
					}
				}
			}
		}

		// Randomly convert some walls in the second-to-last row to passages to make it look less "wally"
		for (unsigned int x = 1; x < width - 1; ++x) {
			if (getFieldType(x, height - 2) == FieldType::WALL) {
				if (RNGEngine::getRandomNumber(1, 3) == 1) {
					delete fields[x][height - 2];
					fields[x][height - 2] = new Passage();
				}
			}
		}
	}
	else {
		if (getFieldType(exit_x, height - 2) == FieldType::WALL) {
			delete fields[exit_x][height - 2];
			fields[exit_x][height - 2] = new Passage();
		}

		bool needsHorizontalConnection = true;

		if (exit_x > 0 && getFieldType(exit_x - 1, height - 2) == FieldType::PASSAGE) {
			needsHorizontalConnection = false;
		}
		if (exit_x < width - 1 && getFieldType(exit_x + 1, height - 2) == FieldType::PASSAGE) {
			needsHorizontalConnection = false;
		}

		if (needsHorizontalConnection) {
			int connectionFound = -1;

			for (int offset = 1; offset < (int)width; ++offset) {
				if (exit_x + offset < width - 1) {
					if (getFieldType(exit_x + offset, height - 2) == FieldType::PASSAGE) {
						connectionFound = exit_x + offset;
						break;
					}
				}
				if (exit_x >= offset && exit_x - offset > 0) {
					if (getFieldType(exit_x - offset, height - 2) == FieldType::PASSAGE) {
						connectionFound = exit_x - offset;
						break;
					}
				}
			}

			if (connectionFound != -1) {
				int start = (exit_x < connectionFound) ? exit_x : connectionFound;
				int end = (exit_x > connectionFound) ? exit_x : connectionFound;

				for (int x = start; x <= end; ++x) {
					if (x > 0 && x < width - 1) {
						delete fields[x][height - 2];
						fields[x][height - 2] = new Passage();
					}
				}
			}
		}
	}
}

MatrixField* Matrix::createRandomItem() const {
	unsigned int itemChoice = RNGEngine::getRandomNumber(1, 4);

	switch (itemChoice) {
	case 1: return new Sword();
	case 2: return new Shield();
	case 3: return new Hammer();
	case 4: return new FogOfWar();
	default: return new Sword();
	}
}

void Matrix::placeItems(unsigned int no_of_items, unsigned int robot_x, unsigned int robot_y) {
	vector<pair<unsigned int, unsigned int>> availablePositions;

	for (unsigned int x = 1; x < width - 1; ++x) {
		for (unsigned int y = 1; y < height - 1; ++y) {
			if (x == robot_x && y == robot_y) continue;

			if (getFieldType(x, y) == FieldType::PASSAGE) {
				availablePositions.push_back(make_pair(x, y));
			}
		}
	}

	if (availablePositions.empty()) {
		cerr << "Warning: No available positions for items!\n";
		return;
	}

	unsigned int itemsToPlace = min(no_of_items, static_cast<unsigned int>(availablePositions.size()));

	random_shuffle(availablePositions.begin(), availablePositions.end());

	for (unsigned int i = 0; i < itemsToPlace; ++i) {
		unsigned int x = availablePositions[i].first;
		unsigned int y = availablePositions[i].second;

		delete fields[x][y];
		fields[x][y] = createRandomItem();
	}
}

/**
 * @brief This function checks if the minotaur and the robot are placed on the same color cells in the matrix (needed for proper game mechanics).
 * @param robot_x The x coordinate of the robot.
 * @param minotaur_pos The potential (x,y) of the minotaur.
 * @return Returns true if the minotaur can be placed on the potential field.
 */
bool Matrix::minotaurPositionChessboardCheck(unsigned int robot_x, pair<unsigned int, unsigned int> minotaur_pos) const {
	return ((robot_x + 1) % 2 == (minotaur_pos.first + minotaur_pos.second) % 2);
}

pair<unsigned int, unsigned int> Matrix::getRandomPassageForMinotaur(unsigned int robot_x) const {
	vector<pair<unsigned int, unsigned int>> availablePositions;

	for (unsigned int x = 1; x < width - 1; ++x) {
		for (unsigned int y = 1; y < height - 1; ++y) {
			if (x == robot_x && y == 1) continue;

			if (getFieldType(x, y) == FieldType::PASSAGE) {
				availablePositions.push_back(make_pair(x, y));
			}
		}
	}

	if (availablePositions.empty()) {
		cerr << "Warning: No available positions for minotaur!\n";
		return make_pair(-1, -1);
	}

	pair<unsigned int, unsigned int> minotaur_pos = availablePositions[RNGEngine::getRandomNumber(0, static_cast<unsigned int>(availablePositions.size()) - 1)];
	while (!minotaurPositionChessboardCheck(robot_x, minotaur_pos)) {
		minotaur_pos = availablePositions[RNGEngine::getRandomNumber(0, static_cast<unsigned int>(availablePositions.size()) - 1)];
	}
	return minotaur_pos;
}

microseconds Matrix::generateMatrix(unsigned int no_of_items) {
	auto start_time = high_resolution_clock::now();

	pair<unsigned int, unsigned int> entrance_and_exit = setEntranceAndExit();

	generativePrim(entrance_and_exit.first);

	assurePathConnectivity(entrance_and_exit.second);

	placeItems(no_of_items, entrance_and_exit.first, 1);

	auto end_time = high_resolution_clock::now();

	auto duration_microseconds = duration_cast<microseconds>(end_time - start_time);
	auto duration_milliseconds = duration_cast<milliseconds>(end_time - start_time);

	cout << "\x1B[38;2;0;0;155;47m" << " - Quick Trivia: " << ANSICodes::RESET << " Legend says that it took Daedalus only " << duration_milliseconds.count() << " ms ("
		<< duration_microseconds.count() << " microseconds) to build the labyrinth (apparently Zeus helped him)...\n\n";

	return duration_microseconds;
}

void Matrix::printMatrix(unsigned int robot_x, unsigned int robot_y, unsigned int minotaur_x, unsigned int minotaur_y) const {
	for (unsigned int i = 0; i < height; ++i) {
		cout << "  ";
		for (unsigned int j = 0; j < width; ++j) {
			if (robot_x == j && robot_y == i)
				cout << ANSICodes::ROBOT_STYLE << 'R' << ANSICodes::RESET;
			else if (minotaur_x == j && minotaur_y == i)
				cout << ANSICodes::MINOTAUR_STYLE << 'M' << ANSICodes::RESET;
			else {
				if (fields[j][i]->getFieldType() == FieldType::WALL)
					cout << ANSICodes::WALL_STYLE << fields[j][i]->getSymbol() << ANSICodes::RESET;
				else if (fields[j][i]->getFieldType() == FieldType::ITEM)
					cout << ANSICodes::ITEM_STYLE << fields[j][i]->getSymbol() << ANSICodes::RESET;
				else if (fields[j][i]->getFieldType() == FieldType::ENTRANCE)
					cout << ANSICodes::ENTRANCE_STYLE << fields[j][i]->getSymbol() << ANSICodes::RESET;
				else if (fields[j][i]->getFieldType() == FieldType::EXIT)
					cout << ANSICodes::EXIT_STYLE << fields[j][i]->getSymbol() << ANSICodes::RESET;
				else cout << fields[j][i]->getSymbol();
			}
		}

		if (i == 1) { 
			cout << "      o Sword: \x1B[31m        0 0 0" << ANSICodes::RESET;
		}
		else if (i == 3) {
			cout << "      o Shield: \x1B[31m       0 0 0" << ANSICodes::RESET;
		}
		else if (i == 5) {
			cout << "      o Hammer: \x1B[31m       0 0 0" << ANSICodes::RESET;
		} 
		else if (i == 7) { 
			cout << "      o Fog of War: \x1B[31m   0 0 0" << ANSICodes::RESET;
		}

		cout << "\n";
	}
	cout << "\n";
}
unsigned int Matrix::getEntranceX() const {
	for (unsigned int x = 0; x < width; ++x) {
		if (getFieldType(x, 0) == FieldType::ENTRANCE) {
			return x;
		}
	}
}

unsigned int Matrix::getWidth() const { 
	return width; 
}

unsigned int Matrix::getHeight() const { 
	return height; 
}