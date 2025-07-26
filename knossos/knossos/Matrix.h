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
	void generativePrim(unsigned int entrance_x);
	void assurePathConnectivity(unsigned int exit_x);
	void placeItems(unsigned int no_of_items, unsigned int robot_x, unsigned int robot_y);
	MatrixField* createRandomItem() const;

	static random_device rd;
	static mersenne_twister gen;

	static unsigned int getRandomNumber(unsigned int min, unsigned int max);

public:
	Matrix(unsigned int w, unsigned int h);

	~Matrix();

	MatrixField* getField(unsigned int x, unsigned int y) const;
	
	FieldType getFieldType(unsigned int x, unsigned int y) const;
	
	void generateMatrix(unsigned int no_of_items);

	void printMatrix() const;
};