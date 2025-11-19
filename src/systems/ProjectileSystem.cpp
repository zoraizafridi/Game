#include "../systems/ProjectileSystem.hpp"
#include "../systems/EnemySystem.hpp"
#include "../systems/ParticleSystem.hpp"
#include "../entities/Enemy.hpp"
#include <cmath>
#include <iostream>
ProjectileSystem::ProjectileSystem(size_t poolSize)
    : projectilePool_(poolSize), enemySystem_(nullptr), particleSystem_(nullptr) {
}
void ProjectileSystem::initialize(EnemySystem* enemySystem, ParticleSystem* particleSystem) {
    enemySystem_ = enemySystem;
    particleSystem_ = particleSystem; // PHASE 3: Store particle system
}
void ProjectileSystem::spawn(const ProjectileInfo& projectile, const sf::Vector2f& position) {
    int index = projectilePool_.allocate();
    if (index == -1) return;
    ProjectileData& data = projectilePool_.get(index);
    data.direction = projectile.direction;
    data.speed = projectile.speed;
    data.damage = projectile.damage;
    data.active = true;
    data.atlas = projectile.atlas;
    data.pos = position;
    data.startPos = position;
    data.distanceTraveled = 0.f;   
    // PHASE 3: Determine type based on atlas with enhanced properties
    if (projectile.atlas.find("fireball") != std::string::npos) {
        data.type = ProjectileData::FIREBALL;
        data.hasTrail = true;
        data.trailInterval = 0.02f;
        data.maxDistance = 800.f;
        data.explodesOnImpact = true;
        data.explosionRadius = 60.f;
        data.rotationSpeed = 180.f; // Spinning fireball
    } else if (projectile.atlas.find("ice") != std::string::npos) {
        data.type = ProjectileData::ICE_SHARD;
        data.hasTrail = true;
        data.trailInterval = 0.04f;
        data.appliesStatusEffect = true;
        data.statusEffectType = 0; // Slow
        data.statusDuration = 3.0f;
        data.rotationSpeed = 90.f;
    } else if (projectile.atlas.find("poison") != std::string::npos) {
        data.type = ProjectileData::POISON_DART;
        data.hasTrail = false;
        data.appliesStatusEffect = true;
        data.statusEffectType = 3; // Poison
        data.statusDuration = 5.0f;
    } else if (projectile.atlas.find("lightning") != std::string::npos) {
        data.type = ProjectileData::LIGHTNING;
        data.hasTrail = true;
        data.trailInterval = 0.01f;
        data.speed = 450.0f;
        data.piercesTargets = true;
        data.maxPierce = 3;
        data.rotationSpeed = 360.f;
    } else if (projectile.atlas.find("cannonball") != std::string::npos) {
        data.type = ProjectileData::CANNONBALL;
        data.hasTrail = true;
        data.trailInterval = 0.05f;
        data.speed = 200.0f;
        data.explodesOnImpact = true;
        data.explosionRadius = 80.f;
    } else if (projectile.atlas.find("ballista") != std::string::npos) {
        data.type = ProjectileData::BALLISTA_BOLT;
        data.hasTrail = true;
        data.trailInterval = 0.03f;
        data.speed = 600.0f;
        data.piercesTargets = true;
        data.maxPierce = 5;
    } else if (projectile.atlas.find("arcane") != std::string::npos) {
        data.type = ProjectileData::ARCANE_ORB;
        data.hasTrail = true;
        data.trailInterval = 0.025f;
        data.rotationSpeed = 120.f;
    } else {
        // Default to arrow
        data.type = ProjectileData::ARROW;
        data.hasTrail = true;
        data.trailInterval = 0.03f;
        data.rotationSpeed = 0.f; // Arrows don't spin
    }
}
// PHASE 3: Specific projectile spawn methods
void ProjectileSystem::spawnArrow(const sf::Vector2f& position, const sf::Vector2f& direction, int damage) {
    ProjectileInfo info;
    info.direction = direction;
    info.speed = 400.0f;
    info.damage = damage;
    info.atlas = "projectile_arrow";
    spawn(info, position);
}
void ProjectileSystem::spawnFireball(const sf::Vector2f& position, const sf::Vector2f& direction, int damage) {
    ProjectileInfo info;
    info.direction = direction;
    info.speed = 250.0f;
    info.damage = damage;
    info.atlas = "projectile_fireball";
    spawn(info, position);
}
void ProjectileSystem::spawnIceShard(const sf::Vector2f& position, const sf::Vector2f& direction, int damage) {
    ProjectileInfo info;
    info.direction = direction;
    info.speed = 350.0f;
    info.damage = damage;
    info.atlas = "projectile_ice_shard";
    spawn(info, position);
}
void ProjectileSystem::spawnPoisonDart(const sf::Vector2f& position, const sf::Vector2f& direction, int damage) {
    ProjectileInfo info;
    info.direction = direction;
    info.speed = 300.0f;
    info.damage = damage;
    info.atlas = "projectile_poison_dart";
    spawn(info, position);
}
void ProjectileSystem::spawnLightning(const sf::Vector2f& position, const sf::Vector2f& direction, int damage) {
    ProjectileInfo info;
    info.direction = direction;
    info.speed = 500.0f;
    info.damage = damage;
    info.atlas = "projectile_lightning";
    spawn(info, position);
}
void ProjectileSystem::spawnCannonball(const sf::Vector2f& position, const sf::Vector2f& direction, int damage) {
    ProjectileInfo info;
    info.direction = direction;
    info.speed = 200.0f;
    info.damage = damage;
    info.atlas = "projectile_cannonball";
    spawn(info, position);
}
void ProjectileSystem::update(float dt) {
    updateMovement(dt);
    updateVisuals(dt);
    updateRotation(dt);
    checkCollisions();
}
void ProjectileSystem::updateMovement(float dt) {
    std::vector<ProjectileData>& projectiles = projectilePool_.raw();
    std::vector<bool>& activeFlags = projectilePool_.activeFlags();
    for (size_t i = 0; i < projectiles.size(); ++i) {
        if (activeFlags[i] && projectiles[i].active) {
            ProjectileData& proj = projectiles[i];
            // Move projectile
            sf::Vector2f movement = proj.direction * proj.speed * dt;
            proj.pos += movement;
            // Update distance traveled
            proj.distanceTraveled += std::sqrt(movement.x * movement.x + movement.y * movement.y);
            // Deactivate if out of bounds or max distance reached
            if (proj.pos.x < -100 || proj.pos.x > 2000 ||
                proj.pos.y < -100 || proj.pos.y > 2000 ||
                proj.distanceTraveled > proj.maxDistance) {
                proj.active = false;
                projectilePool_.free(static_cast<int>(i));
            }
        }
    }
}
void ProjectileSystem::updateVisuals(float dt) {
    if (!particleSystem_) return;
    std::vector<ProjectileData>& projectiles = projectilePool_.raw();
    std::vector<bool>& activeFlags = projectilePool_.activeFlags();
    for (size_t i = 0; i < projectiles.size(); ++i) {
        if (activeFlags[i] && projectiles[i].active) {
            ProjectileData& proj = projectiles[i];
            // Emit trail particles
            if (proj.hasTrail) {
                proj.trailTimer += dt;
                if (proj.trailTimer >= proj.trailInterval) {
                    proj.trailTimer = 0.f;                  
                    // Emit different trail types based on projectile
                    switch (proj.type) {
                        case ProjectileData::FIREBALL:
                            particleSystem_->emit(proj.pos, Particle::FIRE, 1);
                            break;
                        case ProjectileData::ICE_SHARD:
                            particleSystem_->emit(proj.pos, Particle::FROST, 1);
                            break;
                        case ProjectileData::LIGHTNING:
                            particleSystem_->emit(proj.pos, Particle::ELECTRIC, 1);
                            break;
                        case ProjectileData::CANNONBALL:
                            particleSystem_->emit(proj.pos, Particle::SMOKE, 1);
                            break;
                        default:
                            particleSystem_->emit(proj.pos, Particle::SMOKE, 1);
                            break;
                    }
                }
            }
        }
    }
}
void ProjectileSystem::updateRotation(float dt) {
    std::vector<ProjectileData>& projectiles = projectilePool_.raw();
    std::vector<bool>& activeFlags = projectilePool_.activeFlags();
    for (size_t i = 0; i < projectiles.size(); ++i) {
        if (activeFlags[i] && projectiles[i].active) {
            ProjectileData& proj = projectiles[i];
            proj.rotation += proj.rotationSpeed * dt;
        }
    }
}
void ProjectileSystem::checkCollisions() {
    if (!enemySystem_) return;
    std::vector<ProjectileData>& projectiles = projectilePool_.raw();
    std::vector<bool>& activeFlags = projectilePool_.activeFlags();
    for (size_t i = 0; i < projectiles.size(); ++i) {
        if (activeFlags[i] && projectiles[i].active) {
            ProjectileData& proj = projectiles[i];
            auto hitEnemy = enemySystem_->getEnemyAtPosition(proj.pos, 8.0f);
            if (hitEnemy && hitEnemy->health->alive()) {
                handleProjectileImpact(i, proj.pos);
                // Apply damage
                hitEnemy->health->hp -= proj.damage;               
                // Apply status effects if any
                if (proj.appliesStatusEffect) {
                    // This would connect to StatusEffectSystem
                    std::cout << "Applied status effect " << proj.statusEffectType 
                              << " for " << proj.statusDuration << " seconds\n";
                }
                // Check if projectile should be destroyed
                if (!proj.piercesTargets || proj.targetsPierced >= proj.maxPierce) {
                    proj.active = false;
                    projectilePool_.free(static_cast<int>(i));
                } else {
                    proj.targetsPierced++;
                }
            }
        }
    }
}
void ProjectileSystem::handleProjectileImpact(size_t index, const sf::Vector2f& impactPos) {
    ProjectileData& proj = projectilePool_.get(static_cast<int>(index));
    // Create impact effect
    createImpactEffect(proj.type, impactPos);
    // Handle explosion if applicable
    if (proj.explodesOnImpact && particleSystem_) {
        particleSystem_->emitExplosion(impactPos, proj.explosionRadius);
        // Damage all enemies in explosion radius
        if (enemySystem_) {
            auto enemiesInRange = enemySystem_->getEnemiesInRange(impactPos, proj.explosionRadius);
            for (auto& enemy : enemiesInRange) {
                if (enemy->health->alive()) {
                    // Reduced damage for AoE
                    enemy->health->hp -= proj.damage / 2;
                }
            }
        }
    }
}
void ProjectileSystem::createImpactEffect(ProjectileData::Type type, const sf::Vector2f& position) {
    if (!particleSystem_) return;
    switch (type) {
        case ProjectileData::FIREBALL:
            particleSystem_->emitFireEffect(position);
            particleSystem_->emitExplosion(position, 40.0f);
            break;
        case ProjectileData::ICE_SHARD:
            particleSystem_->emitFrostEffect(position);
            break;
        case ProjectileData::POISON_DART:
            particleSystem_->emitPoisonCloud(position);
            break;
        case ProjectileData::LIGHTNING:
            particleSystem_->emit(position, Particle::ELECTRIC, 10);
            break;
        case ProjectileData::CANNONBALL:
            particleSystem_->emitExplosion(position, 60.0f);
            break;
        case ProjectileData::ARROW:
            particleSystem_->emit(position, Particle::SPARKLE, 3);
            break;
        default:
            particleSystem_->emit(position, Particle::SPARKLE, 2);
            break;
    }
}
void ProjectileSystem::clear() {
    std::vector<ProjectileData>& projectiles = projectilePool_.raw();
    std::vector<bool>& activeFlags = projectilePool_.activeFlags();
    for (size_t i = 0; i < projectiles.size(); ++i) {
        if (activeFlags[i]) {
            projectiles[i].active = false;
            projectilePool_.free(static_cast<int>(i));
        }
    }
}
const std::vector<ProjectileData>& ProjectileSystem::getProjectiles() const {
    return projectilePool_.raw();
}