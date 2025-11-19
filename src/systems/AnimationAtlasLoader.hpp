#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "../systems/AnimationSystem.hpp"
class AnimationAtlasLoader {
public:
    static void loadAllAtlases(AnimationSystem* animSystem);
private:
    static void loadEnemyAnimations(AnimationSystem* animSystem);
    static void loadTowerAnimations(AnimationSystem* animSystem);
    static void loadUnitAnimations(AnimationSystem* animSystem);
    static void loadProjectileAnimations(AnimationSystem* animSystem);
};