#pragma once
#include <memory>
// Forward declaration ONLY
class Enemy;
struct TowerAI {
    float cooldown = 0.f;
    std::shared_ptr<Enemy> currentTarget = nullptr;
};