#include "../systems/EnemySystem.hpp"
#include "../entities/Enemy.hpp"
#include "../systems/ProjectileSystem.hpp"
#include "../systems/UnitSystem.hpp"
#include "../core/EventBus.hpp"         // ADD THIS
#include "../core/GameEvents.hpp"       // ADD THIS
#include "../components/Health.hpp"
#include "../components/PathFollower.hpp"
#include "../components/Stats.hpp"
#include "../components/EnemyAI.hpp"
#include "../components/Transform.hpp"
#include "../components/ColliderComp.hpp"
#include "../components/AnimationStateComp.hpp"
#include <algorithm>
#include <cmath>
#include <iostream>

EnemySystem::EnemySystem()
    : projectileSystem_(nullptr), unitSystem_(nullptr), 
      eventBus_(nullptr), aliveCount_(0) {
}

void EnemySystem::initialize(ProjectileSystem* projectileSystem, UnitSystem* unitSystem) {
    projectileSystem_ = projectileSystem;
    unitSystem_ = unitSystem;
}

// ADD THIS NEW METHOD
void EnemySystem::setEventBus(EventBus* eventBus) {
    eventBus_ = eventBus;
    std::cout << "[EnemySystem] EventBus connected" << std::endl;
}

void EnemySystem::add(std::shared_ptr<Enemy> enemy) {
    enemies_.push_back(enemy);
    aliveCount_++;
    std::cout << "[EnemySystem] Added enemy. Total alive: " << aliveCount_ << std::endl;
}

void EnemySystem::update(float dt) {
    updateMovement(dt);
    updateCombat(dt);
    checkEnemyEndReached();
    removeDead();
}

void EnemySystem::updateMovement(float dt) {
    for (auto& enemy : enemies_) {
        if (!enemy->health->alive()) continue;
        
        if (enemy->path->hasPath() && !enemy->path->finished) {
            sf::Vector2f target = enemy->path->getCurrentTarget();
            sf::Vector2f direction = target - enemy->transform->position;
            float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
            
            if (distance < enemy->path->arrivalThreshold) {
                enemy->path->currentIndex++;
                if (enemy->path->currentIndex >= enemy->path->path.size()) {
                    enemy->path->finished = true;
                    std::cout << "[EnemySystem] ⚠ Enemy reached end of path!" << std::endl;
                    
                    // PUBLISH EVENT THROUGH EVENTBUS
                    if (eventBus_) {
                        std::cout << "[EnemySystem] Publishing ENEMY_REACHED_END event" << std::endl;
                        eventBus_->publish(Events::ENEMY_REACHED_END, nullptr);
                    }
                    
                    // Also call the callback for backward compatibility
                    if (onEnemyReachedEnd_) {
                        onEnemyReachedEnd_(enemy);
                    }
                }
            } else {
                if (distance > 0.1f) {
                    direction /= distance;
                    enemy->transform->position += direction * enemy->ai->pathSpeed * dt;
                }
            }
        }
    }
}

void EnemySystem::updateCombat(float dt) {
    for (auto& enemy : enemies_) {
        if (!enemy->health->alive()) continue;
        
        if (enemy->ai->attackRange > 0 && enemy->ai->cooldown <= 0) {
            enemy->ai->cooldown = enemy->ai->attackCooldown;
        }
        
        if (enemy->ai->cooldown > 0) enemy->ai->cooldown -= dt;
    }
}

void EnemySystem::checkEnemyEndReached() {
    for (auto& enemy : enemies_) {
        if (enemy->path->finished && enemy->health->alive()) {
            std::cout << "[EnemySystem] Enemy at end - marking as dead" << std::endl;
            enemy->health->hp = 0;
        }
    }
}

void EnemySystem::removeDead() {
    int removedCount = 0;
    enemies_.erase(std::remove_if(enemies_.begin(), enemies_.end(),
        [this, &removedCount](const std::shared_ptr<Enemy>& enemy) {
            if (!enemy->health->alive()) {
                std::cout << "[EnemySystem] Removing dead enemy" << std::endl;
                
                // Check if enemy died from reaching end vs being killed
                bool reachedEnd = enemy->path && enemy->path->finished;
                
                if (!reachedEnd) {
                    // Enemy was killed, give gold reward
                    int goldReward = 10 + (enemy->health->maxHp / 10);
                    
                    // PUBLISH ENEMY_DIED EVENT
                    if (eventBus_) {
                        EnemyEventData data;
                        data.enemy = enemy;
                        data.goldReward = goldReward;
                        eventBus_->publish(Events::ENEMY_DIED, &data);
                    }
                    
                    // Also call callback for backward compatibility
                    if (onEnemyDied_) {
                        onEnemyDied_(enemy);
                    }
                }
                
                aliveCount_--;
                removedCount++;
                return true;
            }
            return false;
        }), enemies_.end());
    
    if (removedCount > 0) {
        std::cout << "[EnemySystem] Removed " << removedCount << " enemies. Alive: " << aliveCount_ << std::endl;
    }
}

std::shared_ptr<Enemy> EnemySystem::getEnemyAtPosition(const sf::Vector2f& position, float radius) {
    for (auto& enemy : enemies_) {
        if (!enemy->health->alive()) continue;
        
        sf::Vector2f diff = enemy->transform->position - position;
        float distanceSq = diff.x * diff.x + diff.y * diff.y;
        float combinedRadius = enemy->collider->radius + radius;
        
        if (distanceSq < combinedRadius * combinedRadius) return enemy;
    }
    return nullptr;
}

std::vector<std::shared_ptr<Enemy>> EnemySystem::getEnemiesInRange(const sf::Vector2f& position, float range) {
    std::vector<std::shared_ptr<Enemy>> result;
    float rangeSq = range * range;
    
    for (auto& enemy : enemies_) {
        if (!enemy->health->alive()) continue;
        
        sf::Vector2f diff = enemy->transform->position - position;
        float distanceSq = diff.x * diff.x + diff.y * diff.y;
        
        if (distanceSq <= rangeSq) result.push_back(enemy);
    }
    return result;
}

void EnemySystem::setOnEnemyDied(std::function<void(std::shared_ptr<Enemy>)> callback) {
    onEnemyDied_ = callback;
}

void EnemySystem::setOnEnemyReachedEnd(std::function<void(std::shared_ptr<Enemy>)> callback) {
    onEnemyReachedEnd_ = callback;
}