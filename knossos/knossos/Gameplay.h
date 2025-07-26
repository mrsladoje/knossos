#pragma once

#include "Matrix.h"


class Gameplay {
private:
	unsigned int width;
	unsigned int height;
	unsigned int robot_x;
	unsigned int robot_y;
	unsigned int minotaur_x;
	unsigned int minotaur_y;
	unsigned int sword_rounds_left;
	unsigned int shield_rounds_left;
	unsigned int hammer_rounds_left;
	unsigned int fog_of_war_rounds_left;
	Matrix* matrix;

public:
	Gameplay(unsigned int width, unsigned int height)
		: width(width), height(height),
		robot_x(0), robot_y(0),
		minotaur_x(0), minotaur_y(0),
		sword_rounds_left(0), shield_rounds_left(0),
		hammer_rounds_left(0), fog_of_war_rounds_left(0), matrix(nullptr) {}

	~Gameplay() {
		delete matrix;
	}

	void initializeGame(unsigned int no_of_items);
	
};