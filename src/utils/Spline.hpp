#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>
using Vec2 = sf::Vector2f;
class Spline {
public:
    static std::vector<Vec2> catmullRom(const std::vector<Vec2>& pts, int samples);
};
