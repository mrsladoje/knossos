#pragma once

#include <random>
#include "MatrixField.h"

#define mersenne_twister mt19937

using std::random_device;
using std::mersenne_twister;
using std::pair;

class Matrix {
private:
	unsigned int width;
	unsigned int height;
	MatrixField*** fields;

	pair<unsigned int, unsigned int> setEntranceAndExit();
	bool isBoundaryOrOutside(unsigned int x, unsigned int y) const;
	bool minotaurPositionChessboardCheck(unsigned int robot_x, pair<unsigned int, unsigned int> minotaur_pos) const;
	void generativePrim(unsigned int entrance_x);
	void assurePathConnectivity(unsigned int exit_x);
	void placeItems(unsigned int no_of_items, unsigned int robot_x, unsigned int robot_y);
	MatrixField* createRandomItem() const;

	static random_device rd;
	static mersenne_twister gen;

public:
	Matrix(unsigned int w, unsigned int h);

	~Matrix();

	MatrixField* getField(unsigned int x, unsigned int y) const;
	
	FieldType getFieldType(unsigned int x, unsigned int y) const;

	void setField(unsigned int x, unsigned int y, FieldType fieldType);
	
	void generateMatrix(unsigned int no_of_items);

	void printMatrix(unsigned int robot_x, unsigned int robot_y, unsigned int minotaur_x, unsigned int minotaur_y) const;

	pair<unsigned int, unsigned int> getRandomPassageForMinotaur(unsigned int robot_x) const;
	
	unsigned int getEntranceX() const;

	static unsigned int getRandomNumber(unsigned int min, unsigned int max);
};