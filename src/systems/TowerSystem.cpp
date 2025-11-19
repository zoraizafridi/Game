#include "../systems/TowerSystem.hpp"
#include "../entities/Tower.hpp"
#include "../entities/Enemy.hpp"  // INCLUDE HERE in .cpp file
#include "../systems/EnemySystem.hpp"
#include "../systems/ProjectileSystem.hpp"
#include "../components/Transform.hpp"
#include "../components/TowerAI.hpp"
#include "../components/Stats.hpp"
#include "../components/AnimationStateComp.hpp"
#include "../components/ProjectileInfo.hpp"
#include "../components/Health.hpp"
#include <algorithm>
#include <cmath>
TowerSystem::TowerSystem()
    : enemySystem_(nullptr), projectileSystem_(nullptr) {
}
void TowerSystem::initialize(EnemySystem* enemySystem, ProjectileSystem* projectileSystem) {
    enemySystem_ = enemySystem;
    projectileSystem_ = projectileSystem;
}
void TowerSystem::add(std::shared_ptr<Tower> tower) {
    towers_.push_back(tower);
}
void TowerSystem::update(float dt) {
    updateTargeting();
    updateCombat(dt);
}
void TowerSystem::updateTargeting() {
    if (!enemySystem_) return;
    for (auto& tower : towers_) {
        // Check if current target is still valid
        if (!tower->ai->currentTarget || !tower->ai->currentTarget->health->alive()) {
            // Find new target
            auto enemiesInRange = enemySystem_->getEnemiesInRange(
                tower->transform->position, 
                tower->stats->attackRange
            );
            if (!enemiesInRange.empty()) {
                tower->ai->currentTarget = enemiesInRange[0];
            } else {
                tower->ai->currentTarget = nullptr;
            }
        }
    }
}
void TowerSystem::updateCombat(float dt) {
    if (!projectileSystem_) return;    
    for (auto& tower : towers_) {
        // Update cooldown
        if (tower->ai->cooldown > 0) {
            tower->ai->cooldown -= dt;
        }
        // Check if can attack
        if (tower->ai->cooldown <= 0 && 
            tower->ai->currentTarget && 
            tower->ai->currentTarget->health->alive()) {
            // Calculate distance to target
            float dx = tower->transform->position.x - tower->ai->currentTarget->transform->position.x;
            float dy = tower->transform->position.y - tower->ai->currentTarget->transform->position.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            // Check if target is in range
            if (distance <= tower->stats->attackRange) {
                // Create and spawn projectile
                ProjectileInfo info;
                info.direction = tower->ai->currentTarget->transform->position - tower->transform->position;
                // Normalize direction
                float len = std::sqrt(info.direction.x * info.direction.x + info.direction.y * info.direction.y);
                if (len > 0.0f) {
                    info.direction /= len;
                }
                info.speed = 400.0f;
                info.damage = static_cast<int>(tower->stats->damage);
                info.active = true;
                info.atlas = "projectiles";
                projectileSystem_->spawn(info, tower->transform->position);
                // Reset cooldown
                tower->ai->cooldown = 1.0f / tower->stats->attackSpeed;
            }
        }
    }
}
bool TowerSystem::placeTower(std::shared_ptr<Tower> tower, const sf::Vector2f& position) {
    if (!canPlaceTower(position)) {
        return false;
    }   
    tower->transform->position = position;
    towers_.push_back(tower);
    return true;
}
bool TowerSystem::canPlaceTower(const sf::Vector2f& position) const {
    // Check if position is too close to existing towers
    for (const auto& tower : towers_) {
        sf::Vector2f diff = tower->transform->position - position;
        float distanceSq = diff.x * diff.x + diff.y * diff.y;
        float minDistance = 64.0f;
        if (distanceSq < minDistance * minDistance) {
            return false;
        }
    }
    return true;
}
void TowerSystem::setOnTowerUpgraded(std::function<void(std::shared_ptr<Tower>)> callback) {
    onTowerUpgraded_ = callback;
}