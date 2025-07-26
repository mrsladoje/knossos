#include <iostream>
#include <stdexcept>
#include <string>

#include "Matrix.h"
#include "ArgumentsHandler.h"

using std::cout;

int main(int argc, char* argv[])
{
    cout << "\n=== Welcome to the Labyrinth of Knossos, Theseus! ===\n\n";

	unsigned int width, height, no_of_items;
	if (!parseArguments(argc, argv, width, height, no_of_items)) {
		printManual(argv[0]);
		return 1;
	}

	Matrix matrix(width, height);
	matrix.generateMatrix(no_of_items);
	cout << "A tip from Hermes: move the robot using WASD\n\n\n";
	matrix.printMatrix();

	cout << "\n\n";

	return 0;
}