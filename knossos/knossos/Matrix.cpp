#include <iostream>
#include <random>
#include <set>
#include <chrono>

#include "Matrix.h"
#include "MatrixField.h"

#define mersenne_twister mt19937

using std::random_device;
using std::mersenne_twister;
using std::uniform_int_distribution;
using std::vector;
using std::set;
using std::pair;
using std::make_pair;
using std::cout;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::microseconds;
using std::chrono::milliseconds;
using std::min;
using std::random_shuffle;

random_device Matrix::rd;
mersenne_twister Matrix::gen(Matrix::rd());

unsigned int Matrix::getRandomNumber(unsigned int min, unsigned int max) {
	uniform_int_distribution<unsigned int> distrib(min, max);
	return distrib(gen);
}

Matrix::Matrix(unsigned int w, unsigned int h)
    : width(w), height(h), fields(nullptr) {

    fields = new MatrixField**[width];

    for (unsigned int i = 0; i < width; ++i) {
        fields[i] = new MatrixField*[height]; 
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

bool Matrix::isBoundaryOrOutside(unsigned int x, unsigned int y) const {
	return (x <= 0 || x >= width - 1 || y <= 0 || y >= height - 1);
}

pair<unsigned int, unsigned int> Matrix::setEntranceAndExit() {
	unsigned int entrance_x = getRandomNumber(1, width - 2);
	unsigned int exit_x = getRandomNumber(1, width - 2);

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
		unsigned int chosenOne = getRandomNumber(0, (unsigned int)frontiers.size() - 1);
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

		pair<unsigned int, unsigned int> chosenPoint = potentialMazeReconnectionPoints[getRandomNumber(0, (unsigned int)potentialMazeReconnectionPoints.size() - 1)];
		
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
				if (getRandomNumber(1, 3) == 1) {
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
	unsigned int itemChoice = getRandomNumber(1, 4);

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
		cout << "Warning: No available positions for items!\n";
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

void Matrix::generateMatrix(unsigned int no_of_items) {
	auto start_time = high_resolution_clock::now();

	pair<unsigned int, unsigned int> entrance_and_exit = setEntranceAndExit();

	generativePrim(entrance_and_exit.first);

	assurePathConnectivity(entrance_and_exit.second);

	placeItems(no_of_items, entrance_and_exit.first, 1);

	auto end_time = high_resolution_clock::now();

	auto duration_microseconds = duration_cast<microseconds>(end_time - start_time);
	auto duration_milliseconds = duration_cast<milliseconds>(end_time - start_time);

	cout << "Quick Trivia: Legend says that it took Daedalus only " << duration_milliseconds.count() << " ms ("
		<< duration_microseconds.count() << " microseconds) to build the labyrinth (apparently Zeus helped him)\n\n";
}

void Matrix::printMatrix() const {
	for (unsigned int i = 0; i < height; ++i) {
		for (unsigned int j = 0; j < width; ++j) {
			std::cout << fields[j][i]->getSymbol();
		}
		std::cout << std::endl;
	}
}