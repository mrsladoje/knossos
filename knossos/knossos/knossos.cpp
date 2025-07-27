#include <iostream>
#include <stdexcept>
#include <string>

#include "Matrix.h"
#include "ArgumentsHandler.h"
#include "Gameplay.h"

using std::cout;

int main(int argc, char* argv[])
{
	unsigned int width, height, no_of_items;

	handleArguments(argc, argv, width, height, no_of_items);

	Gameplay game(width, height);
	game.initializeGame(no_of_items);
	game.startGameLoop();

	return 0;
}