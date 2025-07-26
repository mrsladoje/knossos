#pragma once

#include <string>

using std::string;

void handleArguments(int argc, char* argv[], unsigned int& width, unsigned int& height, unsigned int& items);

bool parseArguments(int argc, char* argv[], unsigned int& width, unsigned int& height, unsigned int& items);

void printManual(const string& programName);