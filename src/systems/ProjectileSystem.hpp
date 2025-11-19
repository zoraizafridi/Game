#pragma once
#include <vector>
#include <functional>
#include "../components/ProjectileInfo.hpp"
#include "../utils/ObjectPool.hpp"
#include <string>
#include <SFML/System/Vector2.hpp>
// Forward declarations
class EnemySystem;
class ParticleSystem;
struct ProjectileData {
    sf::Vector2f direction {1,0};
    float speed = 200.f;
    int damage = 1;
    bool active = false;
    std::string atlas;
    sf::Vector2f pos;
    // PHASE 3: Enhanced visual properties
    sf::Vector2f startPos;
    float distanceTraveled = 0.f;
    float maxDistance = 1000.f;
    bool hasTrail = false;
    float trailTimer = 0.f;
    float trailInterval = 0.05f;
    float rotation = 0.f;
    float rotationSpeed = 0.f;
    // PHASE 3: Projectile types with special properties
    enum Type {
        ARROW,
        FIREBALL,
        ICE_SHARD,
        POISON_DART,
        LIGHTNING,
        CANNONBALL,
        BALLISTA_BOLT,
        ARCANE_ORB
    };
    Type type = ARROW;  
    // PHASE 3: Special effects
    bool explodesOnImpact = false;
    float explosionRadius = 0.f;
    bool piercesTargets = false;
    int targetsPierced = 0;
    int maxPierce = 1;
    bool appliesStatusEffect = false;
    int statusEffectType = 0; // 0=slow, 1=burn, 2=stun, 3=poison
    float statusDuration = 0.f;
};
class ProjectileSystem {
public:
    ProjectileSystem(size_t poolSize = 256);
    void initialize(EnemySystem* enemySystem, ParticleSystem* particleSystem);
    void spawn(const ProjectileInfo& projectile, const sf::Vector2f& position);
    // PHASE 3: Enhanced spawn methods
    void spawnProjectile(ProjectileData::Type type, const sf::Vector2f& position, 
                        const sf::Vector2f& direction, int damage, float speed);
    void spawnArrow(const sf::Vector2f& position, const sf::Vector2f& direction, int damage);
    void spawnFireball(const sf::Vector2f& position, const sf::Vector2f& direction, int damage);
    void spawnIceShard(const sf::Vector2f& position, const sf::Vector2f& direction, int damage);
    void spawnPoisonDart(const sf::Vector2f& position, const sf::Vector2f& direction, int damage);
    void spawnLightning(const sf::Vector2f& position, const sf::Vector2f& direction, int damage);
    void spawnCannonball(const sf::Vector2f& position, const sf::Vector2f& direction, int damage);
    void update(float dt);
    void clear();
    const std::vector<ProjectileData>& getProjectiles() const;
private:
    void updateMovement(float dt);
    void updateVisuals(float dt);
    void updateRotation(float dt);
    void checkCollisions();
    void handleProjectileImpact(size_t index, const sf::Vector2f& impactPos);
    void createImpactEffect(ProjectileData::Type type, const sf::Vector2f& position);
    ObjectPool<ProjectileData> projectilePool_;
    EnemySystem* enemySystem_;
    ParticleSystem* particleSystem_; // PHASE 3: Add particle system reference
};