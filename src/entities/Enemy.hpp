#pragma once
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <cmath>
// Include all components
#include "../components/Transform.hpp"
#include "../components/SpriteComp.hpp"
#include "../components/AnimationStateComp.hpp"
#include "../components/Health.hpp"
#include "../components/Stats.hpp"
#include "../components/PathFollower.hpp"
#include "../components/ColliderComp.hpp"
#include "../components/StatusEffectsComp.hpp"
#include "../components/EnemyAI.hpp"
class Enemy {
public:
    std::shared_ptr<Transform> transform;
    std::shared_ptr<SpriteComp> sprite;
    std::shared_ptr<AnimationStateComp> anim;
    std::shared_ptr<HealthComp> health;
    std::shared_ptr<StatsComp> stats;
    std::shared_ptr<PathFollower> path;
    std::shared_ptr<ColliderComp> collider;
    std::shared_ptr<StatusEffectsComp> effects;
    std::shared_ptr<EnemyAI> ai;
    // PHASE 3: Enhanced motion and animation
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    bool isMoving = false;
    bool isAttacking = false;
    bool isDead = false;
    float deathTimer = 0.0f;
    float attackTimer = 0.0f;
    // PHASE 3: Enemy type for different behaviors
    std::string enemyType;
    Enemy();
    ~Enemy() = default;
    void initialize();
    void initializeAsType(const std::string& type);
    void update(float dt);
private:
    // PHASE 3: Enhanced movement with physics
    void updateMovement(float dt);
    void updateCombat(float dt);
    void updateAnimation(float dt);
    void updateDeath(float dt);
    void updateStatusEffects(float dt);  
    // PHASE 3: Animation state management
    std::string getCurrentAnimation() const;
};
// PHASE 3: Inline implementation for better performance
inline std::string Enemy::getCurrentAnimation() const {
    if (isDead) return "death";
    if (isAttacking) return "attack";
    if (isMoving) return "walk";
    return "idle";
}