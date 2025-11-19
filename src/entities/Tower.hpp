#pragma once
#include <memory>
#include <SFML/System/Vector2.hpp>
#include "../components/Transform.hpp"
#include "../components/SpriteComp.hpp"
#include "../components/AnimationStateComp.hpp"
#include "../components/TowerAI.hpp"
#include "../components/UpgradeComp.hpp"
#include "../components/Stats.hpp"
class Tower {
public:
    std::shared_ptr<Transform> transform;
    std::shared_ptr<SpriteComp> sprite;
    std::shared_ptr<AnimationStateComp> anim;
    std::shared_ptr<TowerAI> ai;
    std::shared_ptr<UpgradeComp> upgrade;
    std::shared_ptr<StatsComp> stats;
    // PHASE 3: Enhanced animation states
    bool isAttacking = false;
    bool isBuilding = false;
    float attackTimer = 0.0f;
    float buildTimer = 0.0f;
    std::string towerType;
    Tower();
    void initialize();
    void initializeAsType(const std::string& type);
    void update(float dt);
private:
    void updateAnimation(float dt);
    void updateCombat(float dt);
    std::string getCurrentAnimation() const;
};
inline std::string Tower::getCurrentAnimation() const {
    if (isBuilding) return "build";
    if (isAttacking) return "attack";
    return "idle";
}