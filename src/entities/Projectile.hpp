#pragma once
#include <SFML/System/Vector2.hpp>
#include "../components/ProjectileInfo.hpp"  // FIXED: Added ../ prefix
#include <memory>
#include <string>
// Forward declarations
struct Transform;
struct SpriteComp;
struct ProjectileInfo;
class Projectile {
public:
    std::shared_ptr<Transform> transform;
    std::shared_ptr<SpriteComp> sprite;
    std::shared_ptr<ProjectileInfo> info;
    // Enhanced projectile properties
    bool isActive = false;
    float distanceTraveled = 0.0f;
    float rotation = 0.0f;
    float rotationSpeed = 0.0f;
    Projectile();
    ~Projectile();
    void initialize();
    void update(float dt);
    // Enhanced methods
    void activate(const sf::Vector2f& position, const sf::Vector2f& direction, 
                  ProjectileInfo::Type type, int damage, float speed);
    void deactivate();
    bool shouldDestroy() const;
    // Getters for system integration
    ProjectileInfo::Type getType() const { return info->type; }
    bool hasTrail() const { return info->hasTrail; }
    float getTrailInterval() const { return info->trailInterval; }
    bool explodesOnImpact() const { return info->explodesOnImpact; }
    bool piercesTargets() const { return info->piercesTargets; }
    int getMaxPierce() const { return info->maxPierce; }
    int getTargetsPierced() const { return info->targetsPierced; }
    void incrementTargetsPierced() { info->targetsPierced++; }  
private:
    void updateMovement(float dt);
    void updateRotation(float dt);
    void updateVisuals(float dt);
};