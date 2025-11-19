#pragma once
#include <memory>
// Forward declaration ONLY
class Enemy;
struct EnemyAI {
    float pathSpeed = 50.f;
    float attackRange = 0.f;
    float cooldown = 0.f;
    float attackCooldown = 1.0f;
    std::shared_ptr<Enemy> currentTarget = nullptr;
};