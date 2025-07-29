#pragma once
#include <random>

#define mersenne_twister mt19937

using std::random_device;
using std::mersenne_twister;

class RNGEngine {
private:
    static random_device rd;
    static mersenne_twister gen;

    RNGEngine() = default;

public:
    RNGEngine(const RNGEngine&) = delete;
    RNGEngine& operator=(const RNGEngine&) = delete;

    /**
     * @brief Generate a random number within the specified range [min, max]
     * @param min Minimum value (inclusive)
     * @param max Maximum value (inclusive)
     * @return Random integer in the range [min, max]
     */
    static unsigned int getRandomNumber(unsigned int min, unsigned int max);
};