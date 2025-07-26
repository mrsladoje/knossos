#include <string>
#include <iostream>
#include <stdexcept>

using std::string;
using std::cout;
using std::cerr;

void printManual(const string& programName) {
	cout << "\nWrong arguments - unfortunately the maze guard had to turn you away!\n\n";
    cout << "Usage: " << programName << " <width> <height> <number_of_items>\n\n";
    cout << "Parameters:\n";
    cout << "  width           - Width of the maze (must be > 15)\n";
    cout << "  height          - Height of the maze (must be > 15)\n";
    cout << "  number_of_items - Number of special items to place (must be > 3)\n\n";
    cout << "Example: " << programName << " 25 20 5\n";
}

bool parseArguments(int argc, char* argv[], unsigned int& width,
    unsigned int& height, unsigned int& items) {

    if (argc != 4) {
        return false;
    }

    try {
        width = static_cast<unsigned int>(std::stoul(argv[1]));
        height = static_cast<unsigned int>(std::stoul(argv[2]));
        items = static_cast<unsigned int>(std::stoul(argv[3]));

        if (width <= 15) {
            cerr << "Error: Width must be greater than 15 (provided: " << width << ")\n";
            return false;
        }

        if (height <= 15) {
            cerr << "Error: Height must be greater than 15 (provided: " << height << ")\n";
            return false;
        }

        if (items <= 3) {
            cerr << "Error: Number of items must be greater than 3 (provided: " << items << ")\n";
            return false;
        }

        if (items > width*height/3) {
            cerr << "Error: Too many items... Sorry!\n";
            return false;
        }

        return true;

    }
    catch (const std::invalid_argument& e) {
        cerr << "Error: Invalid number format in arguments\n";
        return false;
    }
    catch (const std::out_of_range& e) {
        cerr << "Error: Number out of range in arguments\n";
        return false;
    }
}

void handleArguments(int argc, char* argv[], unsigned int& width, unsigned int& height, unsigned int& items) {
    if (!parseArguments(argc, argv, width, height, items)) {
        printManual(argv[0]);
        exit(1);
    }
}