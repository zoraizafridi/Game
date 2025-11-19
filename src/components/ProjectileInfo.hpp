#pragma once
#include <SFML/System/Vector2.hpp>
#include <string>
struct ProjectileInfo {
    sf::Vector2f direction {1,0};
    float speed = 200.f;
    int damage = 1;
    bool active = false;
    std::string atlas;
    // PHASE 3: Enhanced projectile properties
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