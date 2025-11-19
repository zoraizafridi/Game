#include "../entities/Tower.hpp"
#include "../entities/Enemy.hpp"  // ADDED: Include Enemy for member access
#include <iostream>
Tower::Tower() {
    transform = std::make_shared<Transform>();
    sprite = std::make_shared<SpriteComp>();
    anim = std::make_shared<AnimationStateComp>();
    ai = std::make_shared<TowerAI>();
    upgrade = std::make_shared<UpgradeComp>();
    stats = std::make_shared<StatsComp>();      
    towerType = "arrow_tower";
}
void Tower::initialize() {
    ai->cooldown = 1.0f;
    upgrade->level = 1;
    upgrade->maxLevel = 3;
    upgrade->cost = 100;
    stats->damage = 25.0f;
    stats->attackRange = 200.0f;
    stats->attackSpeed = 1.0f;
    // Animation setup
    anim->playing = true;
    anim->currentAnim = "idle";
    anim->atlasId = "towers";
    anim->frameCount = 1; // Default static
    anim->frameDuration = 0.1f;   
    // Start with build animation
    isBuilding = true;
    buildTimer = 1.0f; // 1 second build time
}
void Tower::initializeAsType(const std::string& type) {
    towerType = type;
    // Use if-else instead of switch to avoid scope issues
    if (type == "arrow_tower") {
        stats->damage = 15.0f;
        stats->attackRange = 200.0f;
        stats->attackSpeed = 1.5f;
        sprite->textureId = "tower_arrow";
        anim->atlasId = "arrow_tower";
        anim->frameCount = 4;
    } 
    else if (type == "cannon_tower") {
        stats->damage = 50.0f;
        stats->attackRange = 180.0f;
        stats->attackSpeed = 0.8f;
        sprite->textureId = "tower_cannon";
        anim->atlasId = "cannon_tower";
        anim->frameCount = 3;
    } 
    else if (type == "mage_tower") {
        stats->damage = 20.0f;
        stats->attackRange = 250.0f;
        stats->attackSpeed = 1.2f;
        sprite->textureId = "tower_mage";
        anim->atlasId = "mage_tower";
        anim->frameCount = 4;
    }
    else if (type == "ice_tower") {
        stats->damage = 18.0f;
        stats->attackRange = 220.0f;
        stats->attackSpeed = 1.3f;
        sprite->textureId = "tower_ice";
        anim->atlasId = "ice_tower";
        anim->frameCount = 3;
    }
    else if (type == "lightning_tower") {
        stats->damage = 25.0f;
        stats->attackRange = 280.0f;
        stats->attackSpeed = 1.0f;
        sprite->textureId = "tower_lightning";
        anim->atlasId = "lightning_tower";
        anim->frameCount = 4;
    }
    else if (type == "poison_tower") {
        stats->damage = 12.0f;
        stats->attackRange = 240.0f;
        stats->attackSpeed = 1.4f;
        sprite->textureId = "tower_poison";
        anim->atlasId = "poison_tower";
        anim->frameCount = 3;
    }
    else if (type == "ballista") {
        stats->damage = 60.0f;
        stats->attackRange = 350.0f;
        stats->attackSpeed = 0.6f;
        sprite->textureId = "tower_ballista";
        anim->atlasId = "ballista";
        anim->frameCount = 2;
    }
    else if (type == "flame_tower") {
        stats->damage = 70.0f;
        stats->attackRange = 200.0f;
        stats->attackSpeed = 0.9f;
        sprite->textureId = "tower_flame";
        anim->atlasId = "flame_tower";
        anim->frameCount = 4;
    }
    else if (type == "tesla_tower") {
        stats->damage = 15.0f;
        stats->attackRange = 180.0f;
        stats->attackSpeed = 3.0f;
        sprite->textureId = "tower_tesla";
        anim->atlasId = "tesla_tower";
        anim->frameCount = 3;
    }
    else if (type == "arcane_tower") {
        stats->damage = 45.0f;
        stats->attackRange = 300.0f;
        stats->attackSpeed = 1.1f;
        sprite->textureId = "tower_arcane";
        anim->atlasId = "arcane_tower";
        anim->frameCount = 4;
    }
    else if (type == "sniper_tower") {
        stats->damage = 150.0f;
        stats->attackRange = 500.0f;
        stats->attackSpeed = 0.4f;
        sprite->textureId = "tower_sniper";
        anim->atlasId = "sniper_tower";
        anim->frameCount = 2;
    }
    else if (type == "artillery_tower") {
        stats->damage = 120.0f;
        stats->attackRange = 400.0f;
        stats->attackSpeed = 0.3f;
        sprite->textureId = "tower_artillery";
        anim->atlasId = "artillery_tower";
        anim->frameCount = 3;
    }
}
void Tower::update(float dt) {
    updateCombat(dt);
    updateAnimation(dt);
    if (ai->cooldown > 0) {
        ai->cooldown -= dt;
    }
    
    if (attackTimer > 0) {
        attackTimer -= dt;
        if (attackTimer <= 0) {
            isAttacking = false;
        }
    }   
    if (isBuilding) {
        buildTimer -= dt;
        if (buildTimer <= 0) {
            isBuilding = false;
        }
    }
}
void Tower::updateCombat(float dt) {
    if (ai->currentTarget && ai->currentTarget->health->alive() && ai->cooldown <= 0) {
        isAttacking = true;
        attackTimer = 0.4f; // Attack animation duration
        ai->cooldown = 1.0f / stats->attackSpeed;
    }
}
void Tower::updateAnimation(float dt) {
    std::string newAnim = getCurrentAnimation();
    if (newAnim != anim->currentAnim) {
        anim->currentAnim = newAnim;
        anim->currentFrame = 0;
        anim->currentTime = 0;
        anim->playing = true;
    }
    anim->currentTime += dt;
    if (anim->currentTime >= anim->frameDuration) {
        anim->currentTime = 0;
        anim->currentFrame++;   
  
        if (anim->currentFrame >= anim->frameCount) {
            anim->currentFrame = 0; // Always loop
        }
    }
    // Visual effects for different tower states
    if (isBuilding) {
        // Pulsing build effect
        float pulse = 0.8f + (1.0f - buildTimer) * 0.4f;
        sprite->setScale(sf::Vector2f(pulse, pulse));
    } else if (isAttacking) {
        // Attack intensity
        float intensity = 1.0f + std::sin(attackTimer * 20.0f) * 0.2f;
        sprite->setScale(sf::Vector2f(intensity, intensity));
    } else {
        sprite->setScale(sf::Vector2f(1.0f, 1.0f));
    }
}