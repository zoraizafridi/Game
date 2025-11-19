#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
struct SpawnGroup {
    std::string id;
    int count;
    float interval;
    float delay;
};
struct Wave {
    int id = 0;
    std::vector<SpawnGroup> groups;
};