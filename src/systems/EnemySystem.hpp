#pragma once
#include <vector>
#include <memory>
#include <functional>
#include <SFML/System/Vector2.hpp>

// Forward declarations ONLY in headers
class Enemy;
class ProjectileSystem;
class UnitSystem;
class EventBus;  // ADD THIS

class EnemySystem {
public:
    EnemySystem();
    void initialize(ProjectileSystem* projectileSystem, UnitSystem* unitSystem);
    void setEventBus(EventBus* eventBus);  // ADD THIS
    void add(std::shared_ptr<Enemy> enemy);
    void update(float dt);
    void removeDead();
    std::shared_ptr<Enemy> getEnemyAtPosition(const sf::Vector2f& position, float radius);
    std::vector<std::shared_ptr<Enemy>> getEnemiesInRange(const sf::Vector2f& position, float range);
    
    // Keep these for backward compatibility
    void setOnEnemyDied(std::function<void(std::shared_ptr<Enemy>)> callback);
    void setOnEnemyReachedEnd(std::function<void(std::shared_ptr<Enemy>)> callback);
    
    const std::vector<std::shared_ptr<Enemy>>& getEnemies() const { return enemies_; }
    int getAliveCount() const { return aliveCount_; }

private:
    void updateMovement(float dt);
    void updateCombat(float dt);
    void checkEnemyEndReached();
    
    std::vector<std::shared_ptr<Enemy>> enemies_;
    ProjectileSystem* projectileSystem_;
    UnitSystem* unitSystem_;
    EventBus* eventBus_ = nullptr;  // ADD THIS
    
    std::function<void(std::shared_ptr<Enemy>)> onEnemyDied_;
    std::function<void(std::shared_ptr<Enemy>)> onEnemyReachedEnd_;
    
    int aliveCount_ = 0;
};
