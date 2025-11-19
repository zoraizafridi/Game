#include "../entities/Projectile.hpp"
#include "../components/Transform.hpp"
#include "../components/SpriteComp.hpp"
#include "../components/ProjectileInfo.hpp"
#include <cmath>
#include <iostream>
Projectile::Projectile() {
    transform = std::make_shared<Transform>();
    sprite = std::make_shared<SpriteComp>();
    info = std::make_shared<ProjectileInfo>();
}
Projectile::~Projectile() {
    // Cleanup if needed
}
void Projectile::initialize() {
    info->speed = 300.0f;
    info->damage = 15;
    info->active = false;
    info->maxDistance = 1000.f;
    info->hasTrail = false;
    info->trailInterval = 0.05f;
    info->type = ProjectileInfo::ARROW;
    info->explodesOnImpact = false;
    info->explosionRadius = 0.f;
    info->piercesTargets = false;
    info->maxPierce = 1;
    info->targetsPierced = 0;
    info->appliesStatusEffect = false;
    info->statusEffectType = 0;
    info->statusDuration = 0.f;
    isActive = false;
    distanceTraveled = 0.0f;
    rotation = 0.0f;
    rotationSpeed = 0.0f;      
    // Set default sprite properties
    sprite->layer = 5; // Projectiles above most entities
    sprite->visible = true;
}
void Projectile::activate(const sf::Vector2f& position, const sf::Vector2f& direction, 
                         ProjectileInfo::Type type, int damage, float speed) {
    transform->position = position;
    info->direction = direction;
    info->type = type;
    info->damage = damage;
    info->speed = speed;
    info->active = true;
    info->startPos = position;
    info->distanceTraveled = 0.0f;
    info->targetsPierced = 0;
    isActive = true;
    distanceTraveled = 0.0f;
    rotation = 0.0f;
    // Set properties based on projectile type
    switch (type) {
        case ProjectileInfo::ARROW:
            info->hasTrail = true;
            info->trailInterval = 0.03f;
            info->maxDistance = 1200.f;
            rotationSpeed = 0.0f;
            sprite->textureId = "projectile_arrow";
            break;
        case ProjectileInfo::FIREBALL:
            info->hasTrail = true;
            info->trailInterval = 0.02f;
            info->maxDistance = 800.f;
            info->explodesOnImpact = true;
            info->explosionRadius = 60.f;
            rotationSpeed = 180.f;
            sprite->textureId = "projectile_fireball";
            break;
        case ProjectileInfo::ICE_SHARD:
            info->hasTrail = true;
            info->trailInterval = 0.04f;
            info->appliesStatusEffect = true;
            info->statusEffectType = 0; // Slow
            info->statusDuration = 3.0f;
            rotationSpeed = 90.f;
            sprite->textureId = "projectile_ice_shard";
            break;           
        case ProjectileInfo::POISON_DART:
            info->hasTrail = false;
            info->appliesStatusEffect = true;
            info->statusEffectType = 3; // Poison
            info->statusDuration = 5.0f;
            rotationSpeed = 0.0f;
            sprite->textureId = "projectile_poison_dart";
            break;
        case ProjectileInfo::LIGHTNING:
            info->hasTrail = true;
            info->trailInterval = 0.01f;
            info->speed = 450.0f;
            info->piercesTargets = true;
            info->maxPierce = 3;
            rotationSpeed = 360.f;
            sprite->textureId = "projectile_lightning";
            break;
        case ProjectileInfo::CANNONBALL:
            info->hasTrail = true;
            info->trailInterval = 0.05f;
            info->speed = 200.0f;
            info->explodesOnImpact = true;
            info->explosionRadius = 80.f;
            rotationSpeed = 45.f;
            sprite->textureId = "projectile_cannonball";
            break;
        case ProjectileInfo::BALLISTA_BOLT:
            info->hasTrail = true;
            info->trailInterval = 0.03f;
            info->speed = 600.0f;
            info->piercesTargets = true;
            info->maxPierce = 5;
            rotationSpeed = 0.0f;
            sprite->textureId = "projectile_ballista_bolt";
            break;       
        case ProjectileInfo::ARCANE_ORB:
            info->hasTrail = true;
            info->trailInterval = 0.025f;
            rotationSpeed = 120.f;
            sprite->textureId = "projectile_arcane_orb";
            break;
    }       
    // Rotate sprite to face direction
    if (direction.x != 0 || direction.y != 0) {
        float angle = std::atan2(direction.y, direction.x) * 180 / 3.14159f;
        transform->rotation = angle;
    }
}
void Projectile::deactivate() {
    info->active = false;
    isActive = false;
    sprite->visible = false;
}
void Projectile::update(float dt) {
    if (!isActive) return;
    updateMovement(dt);
    updateRotation(dt);
    updateVisuals(dt);      
    // Check if projectile should be destroyed
    if (shouldDestroy()) {
        deactivate();
    }
}
void Projectile::updateMovement(float dt) {
    // Move projectile
    sf::Vector2f movement = info->direction * info->speed * dt;
    transform->position += movement;      
    // Update distance traveled
    distanceTraveled += std::sqrt(movement.x * movement.x + movement.y * movement.y);
    info->distanceTraveled = distanceTraveled;
}
void Projectile::updateRotation(float dt) {
    rotation += rotationSpeed * dt;
    transform->rotation += rotationSpeed * dt;      
    // Keep rotation in 0-360 range
    if (transform->rotation >= 360.0f) transform->rotation -= 360.0f;
    if (transform->rotation < 0.0f) transform->rotation += 360.0f;
}
void Projectile::updateVisuals(float dt) {
    // Update trail timer
    if (info->hasTrail) {
        info->trailTimer += dt;
    }      
    // FIXED: Scale projectiles based on type - use if-else instead of switch
    if (info->type == ProjectileInfo::FIREBALL) {
        // Pulsing fireball effect
        float pulse = std::sin(distanceTraveled * 0.1f) * 0.1f + 1.0f;
        sprite->setScale(sf::Vector2f(pulse, pulse));
    } 
    else if (info->type == ProjectileInfo::ARCANE_ORB) {
        // Rotating arcane orb
        float scale = 0.8f + std::sin(rotation * 0.1f) * 0.2f;
        sprite->setScale(sf::Vector2f(scale, scale));
    } 
    else {
        // Default scale
        sprite->setScale(sf::Vector2f(1.0f, 1.0f));
    }
}
bool Projectile::shouldDestroy() const {
    // Check if out of bounds
    if (transform->position.x < -100 || transform->position.x > 2000 ||
        transform->position.y < -100 || transform->position.y > 2000) {
        return true;
    }
    // Check if exceeded max distance
    if (distanceTraveled > info->maxDistance) {
        return true;
    }
    // Check if exceeded max pierce count
    if (info->piercesTargets && info->targetsPierced >= info->maxPierce) {
        return true;
    }
    return false;
}