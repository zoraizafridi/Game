#pragma once
#include <random>
class Random {
public:
    static float range(float a, float b);
    static int rangeInt(int a, int b);
private:
    static std::mt19937& generator();
};