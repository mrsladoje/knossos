#include <iostream>
#include "Matrix.h"

using std::cout;
using std::endl;

int main()
{
    cout << "\nWelcome Theseus!\n\n";

	unsigned int width = 20, height = 9;

	Matrix matrix(width, height);
	matrix.generateMatrix(3);
	matrix.printMatrix();

	cout << "\n\n";

	return 0;
}