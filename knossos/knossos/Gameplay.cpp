#include <iostream>

#include "Matrix.h"
#include "Gameplay.h"

using std::cout;
using std::pair;

void Gameplay::initializeGame(unsigned int no_of_items) {
	cout << "\n============= Welcome to the Labyrinth of Knossos, Theseus! =============\n\n\n";
	
	matrix = new Matrix(width, height);
	matrix->generateMatrix(no_of_items);
	
	robot_x = matrix->getEntranceX();
	robot_y = 1;

	pair<unsigned int, unsigned int> minotaurPosition = matrix->getRandomPassage(robot_x);
	minotaur_x = minotaurPosition.first;
	minotaur_y = minotaurPosition.second;

	cout << "\n- A swift message from Hermes, messenger of the gods:\n";
	cout << "\n   \"Brave traveler, I guide all who wander through unknown paths.\n";
	cout << "    Use WASD to move your mechanical companion through this labyrinth -\n";
	cout << "    W for north, A for west, S for south, D for east.\n\n";
	cout << "    Move wisely, for speed and cunning shall serve you well here.\n";
	cout << "    May the gods favor your journey!\"\n\n";

	cout << "\n- Hephaestus, god of forge, warns:\n";
	cout << "\n   \"Beware, mortal! I have scattered my crafted relics throughout this maze.\n";
	cout << "    Each 'P' holds a mystery - you won't know what I've forged until you step upon it!\n\n";
	cout << "    My divine creations include:\n";
	cout << "	  * Sword - Sharp enough to cut through even a Minotaur's hide! *forge-ive me the pun*\n";
	cout << "	  * Shield - Defense so strong, it'll make you feel *metal-ly* prepared!\n";
	cout << "	  * Hammer - Breaks walls like my legendary smithing breaks expectations!\n";
	cout << "	  * Fog of War - Clouds your vision... I was having a *mist-ical* day when I made this one!\n\n";
	cout << "    Remember: Each blessing lasts but 3 moves. Use them *smith-ly*!\"\n\n\n";

	matrix->printMatrix(robot_x, robot_y, minotaur_x, minotaur_y);
}