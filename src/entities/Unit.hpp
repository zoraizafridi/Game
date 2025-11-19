#pragma once
#include <SFML/System/Vector2.hpp>
#include <memory>
#include <cmath>
#include "../components/Transform.hpp"
#include "../components/SpriteComp.hpp"
#include "../components/AnimationStateComp.hpp"
#include "../components/Health.hpp"
#include "../components/Stats.hpp"
#include "../components/ColliderComp.hpp"
#include "../components/UnitAI.hpp"
#include "../components/SelectableComp.hpp"
#include "../components/UpgradeComp.hpp"
class Unit {
public:
    std::shared_ptr<Transform> transform;
    std::shared_ptr<SpriteComp> sprite;
    std::shared_ptr<AnimationStateComp> anim;
    std::shared_ptr<HealthComp> health;
    std::shared_ptr<StatsComp> stats;
    std::shared_ptr<ColliderComp> collider;
    std::shared_ptr<UnitAI> ai;
    std::shared_ptr<SelectableComp> selectable;
    std::shared_ptr<UpgradeComp> upgrade;
    // PHASE 3: Enhanced motion and animation
    sf::Vector2f velocity;
    sf::Vector2f acceleration;
    bool isMoving = false;
    bool isAttacking = false;
    bool isDead = false;
    float deathTimer = 0.0f;
    float attackTimer = 0.0f;
    std::string unitType;
    Unit();
    ~Unit() = default;
    void initialize();
    void initializeAsType(const std::string& type);
    void update(float dt);
private:
    void updateMovement(float dt);
    void updateCombat(float dt);
    void updateAnimation(float dt);
    void updateSelectionVisuals(float dt);
    std::string getCurrentAnimation() const;
};
inline std::string Unit::getCurrentAnimation() const {
    if (isDead) return "death";
    if (isAttacking) return "attack";
    if (isMoving) return "walk";
    return "idle";
}