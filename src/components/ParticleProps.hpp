#pragma once
#include <SFML/System/Vector2.hpp>
struct ParticleProps {
    sf::Vector2f velocity;
    float life = 1.f;
};