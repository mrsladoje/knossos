#pragma once

#include "Matrix.h"

using std::pair;
using std::make_pair;

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
	pair<int, int> initial_console_size;
	Matrix* matrix;

	void printMatrixCharacter(char symbol) const;
	void updateMatrixCharacter(unsigned int x, unsigned int y, char symbol) const;
	void positionCursorAtRobot() const;
	void moveMinotaur(unsigned int prev_minotaur_x, unsigned int prev_minotaur_y);
	bool checkGameEndConditions();
	void activateEffect(ItemType itemType);
	void recalculateEffects();
	void fillEffectHearts(unsigned int y, unsigned int no_of_hearts);
	void drawFog() const;
	void redrawMatrixAfterFog() const;
	bool minotaurAlive() const;
	void ariadneCongratulates() const;

public:
	Gameplay(unsigned int width, unsigned int height)
		: width(width), height(height),
		robot_x(0), robot_y(0),
		minotaur_x(0), minotaur_y(0),
		sword_rounds_left(0), shield_rounds_left(0),
		hammer_rounds_left(0), fog_of_war_rounds_left(0), 
		initial_console_size(make_pair(0,0)), matrix(nullptr) {}

	~Gameplay() {
		delete matrix;
	}

	void initializeGame(unsigned int no_of_items);
	void startGameLoop();
	
};