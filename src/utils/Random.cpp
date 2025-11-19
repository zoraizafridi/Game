#include "../utils/Random.hpp"
#include <random>
std::mt19937& Random::generator() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    return gen;
}
float Random::range(float a, float b) {
    std::uniform_real_distribution<float> dist(a, b);
    return dist(generator());
}
int Random::rangeInt(int a, int b) {
    std::uniform_int_distribution<int> dist(a, b);
    return dist(generator());
}