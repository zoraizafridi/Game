#include "../systems/UnitSystem.hpp"
#include "../entities/Unit.hpp"  // INCLUDE HERE in .cpp file
#include "../systems/EnemySystem.hpp"
#include "../entities/Enemy.hpp"
#include "../systems/ProjectileSystem.hpp"
#include "../components/Health.hpp"
#include "../components/Stats.hpp"
#include "../components/UnitAI.hpp"
#include "../components/Transform.hpp"
#include "../components/ColliderComp.hpp"
#include "../components/SelectableComp.hpp"
#include "../components/AnimationStateComp.hpp"
#include "../components/ProjectileInfo.hpp"
#include <algorithm>
#include <cmath>

UnitSystem::UnitSystem()
    : enemySystem_(nullptr), projectileSystem_(nullptr), selectedUnit_(nullptr) {
}

void UnitSystem::initialize(EnemySystem* enemySystem, ProjectileSystem* projectileSystem) {
    enemySystem_ = enemySystem;
    projectileSystem_ = projectileSystem;
}

void UnitSystem::add(std::shared_ptr<Unit> unit) {
    units_.push_back(unit);
}

void UnitSystem::update(float dt) {
    updateTargeting();
    updateCombat(dt);
    removeDead();
}

void UnitSystem::updateTargeting() {
    if (!enemySystem_) return;
    
    for (auto& unit : units_) {
        if (!unit->health->alive()) continue;
        
        // Check if current target is still valid
        if (!unit->ai->currentTarget || !unit->ai->currentTarget->health->alive()) {
            // Find new target in range
            auto enemiesInRange = enemySystem_->getEnemiesInRange(
                unit->transform->position, 
                unit->stats->attackRange
            );
            
            if (!enemiesInRange.empty()) {
                unit->ai->currentTarget = enemiesInRange[0];
            } else {
                unit->ai->currentTarget = nullptr;
            }
        }
    }
}

void UnitSystem::updateCombat(float dt) {
    for (auto& unit : units_) {
        if (!unit->health->alive()) continue;
        
        // Update cooldown
        if (unit->ai->cooldown > 0) {
            unit->ai->cooldown -= dt;
        }
        
        // Check if can attack
        if (unit->ai->cooldown <= 0 && 
            unit->ai->currentTarget && 
            unit->ai->currentTarget->health->alive()) {
            // Calculate distance to target
            float dx = unit->transform->position.x - unit->ai->currentTarget->transform->position.x;
            float dy = unit->transform->position.y - unit->ai->currentTarget->transform->position.y;
            float distance = std::sqrt(dx * dx + dy * dy);
            
            // Check if target is in range
            if (distance <= unit->stats->attackRange) {
                if (!unit->ai->melee && projectileSystem_) {
                    // Ranged attack - spawn projectile
                    ProjectileInfo info;
                    info.direction = unit->ai->currentTarget->transform->position - unit->transform->position;
                    
                    // Normalize direction
                    float len = std::sqrt(info.direction.x * info.direction.x + info.direction.y * info.direction.y);
                    if (len > 0.0f) {
                        info.direction /= len;
                    }
                    
                    info.speed = 300.0f;
                    info.damage = static_cast<int>(unit->stats->damage);
                    info.active = true;
                    info.atlas = "projectiles";
                    
                    projectileSystem_->spawn(info, unit->transform->position);
                    
                } else if (unit->ai->melee) {
                    // Melee attack - direct damage
                    unit->ai->currentTarget->health->hp -= static_cast<int>(unit->stats->damage);
                }
                
                // Reset cooldown
                unit->ai->cooldown = 1.0f / unit->stats->attackSpeed;
            }
        }
    }
}

void UnitSystem::removeDead() {
    units_.erase(std::remove_if(units_.begin(), units_.end(),
        [this](const std::shared_ptr<Unit>& unit) {
            if (!unit->health->alive()) {
                if (onUnitDied_) onUnitDied_(unit);
                if (selectedUnit_ == unit) selectedUnit_ = nullptr;
                return true;
            }
            return false;
        }), units_.end());
}

void UnitSystem::setSelectedUnit(std::shared_ptr<Unit> unit) {
    if (selectedUnit_) {
        selectedUnit_->selectable->selected = false;
    }
    selectedUnit_ = unit;
    if (selectedUnit_) {
        selectedUnit_->selectable->selected = true;
    }
}

void UnitSystem::clearSelection() {
    if (selectedUnit_) {
        selectedUnit_->selectable->selected = false;
        selectedUnit_ = nullptr;
    }
}

std::shared_ptr<Unit> UnitSystem::getUnitAtPosition(const sf::Vector2f& position) {
    for (auto& unit : units_) {
        if (!unit->health->alive()) continue;
        
        sf::Vector2f diff = unit->transform->position - position;
        float distanceSq = diff.x * diff.x + diff.y * diff.y;
        float radiusSq = unit->collider->radius * unit->collider->radius;
        
        if (distanceSq <= radiusSq) {
            return unit;
        }
    }
    return nullptr;
}

std::vector<std::shared_ptr<Unit>> UnitSystem::getUnitsInRange(const sf::Vector2f& position, float range) {
    std::vector<std::shared_ptr<Unit>> result;
    float rangeSq = range * range;
    
    for (auto& unit : units_) {
        if (!unit->health->alive()) continue;

        sf::Vector2f diff = unit->transform->position - position;
        float distanceSq = diff.x * diff.x + diff.y * diff.y;       
        if (distanceSq <= rangeSq) {
            result.push_back(unit);
        }
    }
    return result;
}
void UnitSystem::setOnUnitDied(std::function<void(std::shared_ptr<Unit>)> callback) {
    onUnitDied_ = callback;
}