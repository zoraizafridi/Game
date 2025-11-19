#pragma once
#include <memory>
#include <vector>
#include <functional>
#include <SFML/Graphics.hpp>  // ADD THIS

// Forward declarations
class EnemySystem;
class ProjectileSystem;
class Enemy;

// INCLUDE THE ACTUAL UNIT CLASS DEFINITION
#include "../entities/Unit.hpp"  // ADD THIS LINE

class UnitSystem {
private:
    std::vector<std::shared_ptr<Unit>> units_;
    EnemySystem* enemySystem_;
    ProjectileSystem* projectileSystem_;
    std::shared_ptr<Unit> selectedUnit_;
    std::function<void(std::shared_ptr<Unit>)> onUnitDied_;

public:
    UnitSystem();
    void initialize(EnemySystem* enemySystem, ProjectileSystem* projectileSystem);
    void add(std::shared_ptr<Unit> unit);
    void update(float dt);
    void removeDead();
    
    // Selection methods
    void setSelectedUnit(std::shared_ptr<Unit> unit);
    void clearSelection();
    std::shared_ptr<Unit> getUnitAtPosition(const sf::Vector2f& position);
    std::vector<std::shared_ptr<Unit>> getUnitsInRange(const sf::Vector2f& position, float range);
    
    // Getters
    std::vector<std::shared_ptr<Unit>>& getUnits() { return units_; }
    std::vector<std::shared_ptr<Unit>>& getUnitsModifiable() { return units_; }
    int getAliveCount() const {
        int count = 0;
        for (const auto& unit : units_) {
            if (unit->health->alive()) count++;
        }
        return count;
    }
    
    // Callbacks
    void setOnUnitDied(std::function<void(std::shared_ptr<Unit>)> callback);

private:
    void updateTargeting();
    void updateCombat(float dt);
};