#include "RNGEngine.h"

#define mersenne_twister mt19937

using std::uniform_int_distribution;
using std::random_device;
using std::mersenne_twister;


random_device RNGEngine::rd;
mersenne_twister RNGEngine::gen(RNGEngine::rd());

unsigned int RNGEngine::getRandomNumber(unsigned int min, unsigned int max) {
	uniform_int_distribution<unsigned int> distrib(min, max);
	return distrib(gen);
}