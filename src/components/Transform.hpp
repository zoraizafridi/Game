#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>
struct Transform {
    sf::Vector2f position;
    sf::Vector2f scale;
    float rotation;  // PHASE 3: Add rotation
    Transform(sf::Vector2f pos = {0, 0}) 
        : position(pos), scale({1, 1}), rotation(0) {}
    // PHASE 3: Rotation helper methods
    void lookAt(const sf::Vector2f& target) {
        sf::Vector2f direction = target - position;
        rotation = std::atan2(direction.y, direction.x) * 180 / 3.14159f;
    }
    void setRotation(float angle) {
        rotation = angle;
    }
};