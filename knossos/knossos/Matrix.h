#pragma once

#include <chrono>
#include "MatrixField.h"

using std::pair;
using std::chrono::microseconds;

class Matrix {
private:
	unsigned int width;
	unsigned int height;
	MatrixField*** fields;

	pair<unsigned int, unsigned int> setEntranceAndExit();
	bool minotaurPositionChessboardCheck(unsigned int robot_x, pair<unsigned int, unsigned int> minotaur_pos) const;
	void generativePrim(unsigned int entrance_x);
	void assurePathConnectivity(unsigned int exit_x);
	void placeItems(unsigned int no_of_items, unsigned int robot_x, unsigned int robot_y);
	MatrixField* createRandomItem() const;

public:

	Matrix(unsigned int w, unsigned int h);
	~Matrix();

	bool isBoundaryOrOutside(unsigned int x, unsigned int y) const;
	MatrixField* getField(unsigned int x, unsigned int y) const;
	FieldType getFieldType(unsigned int x, unsigned int y) const;
	void setField(unsigned int x, unsigned int y, FieldType fieldType);
	microseconds generateMatrix(unsigned int no_of_items);
	void printMatrix(unsigned int robot_x, unsigned int robot_y, unsigned int minotaur_x, unsigned int minotaur_y) const;
	pair<unsigned int, unsigned int> getRandomPassageForMinotaur(unsigned int robot_x) const;
	unsigned int getEntranceX() const;
	unsigned int getWidth() const;
	unsigned int getHeight() const;
};