#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <SFML/System/Vector2.hpp>
// Forward declarations ONLY
class Tower;
class EnemySystem;
class ProjectileSystem;
class TowerSystem {
public:
    TowerSystem();
    void initialize(EnemySystem* enemySystem, ProjectileSystem* projectileSystem);
    void add(std::shared_ptr<Tower> tower);
    void update(float dt);
    bool placeTower(std::shared_ptr<Tower> tower, const sf::Vector2f& position);
    bool canPlaceTower(const sf::Vector2f& position) const;
    void setOnTowerUpgraded(std::function<void(std::shared_ptr<Tower>)> callback);
    // FIXED: Return const reference for reading
    const std::vector<std::shared_ptr<Tower>>& getTowers() const { return towers_; }
    // ADDED: Return non-const reference for modifying (use carefully)
    std::vector<std::shared_ptr<Tower>>& getTowersModifiable() { return towers_; }
private:
    void updateTargeting();
    void updateCombat(float dt);
    std::vector<std::shared_ptr<Tower>> towers_;
    EnemySystem* enemySystem_;
    ProjectileSystem* projectileSystem_;
    std::function<void(std::shared_ptr<Tower>)> onTowerUpgraded_;
};