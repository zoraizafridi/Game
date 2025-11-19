#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>
struct PathFollower {
    std::vector<sf::Vector2f> path;
    int currentIndex = 0;
    float speed = 50.0f;
    float arrivalThreshold = 5.0f;
    bool loop = false;
    bool finished = false;
    bool hasPath() const { return !path.empty() && currentIndex < path.size(); }
    sf::Vector2f getCurrentTarget() const { 
        return hasPath() ? path[currentIndex] : sf::Vector2f(0, 0); 
    }
};