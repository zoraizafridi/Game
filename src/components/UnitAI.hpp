#pragma once
#include <memory>
// Forward declaration ONLY
class Enemy;
struct UnitAI {
    float cooldown = 0.f;
    bool melee = false;
    float attackRange = 100.f;
    std::shared_ptr<Enemy> currentTarget = nullptr;
};