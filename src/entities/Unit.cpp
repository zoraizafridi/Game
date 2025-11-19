#include "../entities/Unit.hpp"
#include "../entities/Enemy.hpp"  // ADDED: Include Enemy for member access
#include "../components/UnitAI.hpp"
#include <iostream>

Unit::Unit() {
    transform = std::make_shared<Transform>();
    sprite = std::make_shared<SpriteComp>();
    anim = std::make_shared<AnimationStateComp>();
    health = std::make_shared<HealthComp>();
    stats = std::make_shared<StatsComp>();
    collider = std::make_shared<ColliderComp>();
    ai = std::make_shared<UnitAI>();
    selectable = std::make_shared<SelectableComp>();
    upgrade = std::make_shared<UpgradeComp>();
    
    velocity = sf::Vector2f(0, 0);
    acceleration = sf::Vector2f(0, 0);
    unitType = "archer";
}

void Unit::initialize() {
    health->hp = 50;
    health->maxHp = 50;
    stats->speed = 75.0f;
    stats->damage = 20.0f;
    stats->attackRange = 150.0f;
    stats->attackSpeed = 1.5f;
    collider->radius = 10.0f;
    ai->melee = false;
    ai->cooldown = 0.5f;
    ai->attackRange = 150.0f;
    upgrade->level = 1;
    upgrade->maxLevel = 3;
    upgrade->cost = 100;
    
    // Animation setup
    anim->playing = true;
    anim->currentAnim = "idle";
    anim->atlasId = "units";
    anim->frameCount = 4;
    anim->frameDuration = 0.1f;
}

void Unit::initializeAsType(const std::string& type) {
    unitType = type;
    
    if (type == "archer") {
        health->hp = 60;
        health->maxHp = 60;
        stats->speed = 70.0f;
        stats->damage = 25.0f;
        stats->attackRange = 180.0f;
        sprite->textureId = "unit_archer";
        anim->atlasId = "archer";
    } else if (type == "knight") {
        health->hp = 150;
        health->maxHp = 150;
        stats->speed = 50.0f;
        stats->damage = 40.0f;
        stats->attackRange = 40.0f;
        ai->melee = true;
        sprite->textureId = "unit_knight";
        anim->atlasId = "knight";
    } else if (type == "mage") {
        health->hp = 80;
        health->maxHp = 80;
        stats->speed = 55.0f;
        stats->damage = 35.0f;
        stats->attackRange = 220.0f;
        sprite->textureId = "unit_mage";
        anim->atlasId = "mage";
    }
}

void Unit::update(float dt) {
    if (isDead) return;
    
    if (!health->alive()) {
        isDead = true;
        deathTimer = 1.0f;
        velocity = sf::Vector2f(0, 0);
        anim->currentAnim = "death";
        anim->currentFrame = 0;
        anim->currentTime = 0;
        return;
    }
    
    // Reset states
    isMoving = false;
    isAttacking = false;
    
    updateMovement(dt);
    updateCombat(dt);
    updateAnimation(dt);
    updateSelectionVisuals(dt);
    
    if (ai->cooldown > 0.f) {
        ai->cooldown -= dt;
    }
}

void Unit::updateMovement(float dt) {
    // Simple movement towards target if we have one
    if (ai->currentTarget && ai->currentTarget->health->alive()) {
        sf::Vector2f targetPos = ai->currentTarget->transform->position;
        sf::Vector2f direction = targetPos - transform->position;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        
        if (distance > ai->attackRange) {
            // Move towards target
            if (distance > 0.1f) {
                direction /= distance;
                sf::Vector2f desiredVelocity = direction * stats->speed;
                sf::Vector2f steering = desiredVelocity - velocity;
                acceleration = steering * 6.0f;             
                
                velocity += acceleration * dt;
                float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
                if (speed > stats->speed) {
                    velocity = (velocity / speed) * stats->speed;
                }
                
                transform->position += velocity * dt;
                
                // Rotate to face movement direction
                if (speed > 0.1f) {
                    transform->lookAt(transform->position + velocity);
                }
                
                isMoving = true;
            }
        }
    }
}
void Unit::updateCombat(float dt) {
    if (ai->currentTarget && ai->currentTarget->health->alive() && ai->cooldown <= 0) {
        sf::Vector2f targetPos = ai->currentTarget->transform->position;
        sf::Vector2f direction = targetPos - transform->position;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        if (distance <= ai->attackRange) {
            isAttacking = true;
            attackTimer = 0.3f;
            ai->cooldown = 1.0f / stats->attackSpeed;
        }
    }      
    if (attackTimer > 0) {
        attackTimer -= dt;
        if (attackTimer <= 0) {
            isAttacking = false;
        }
    }
}
void Unit::updateAnimation(float dt) {
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
            if (isDead) {
                anim->currentFrame = anim->frameCount - 1;
                anim->playing = false;
            } else {
                anim->currentFrame = 0;
            }
        }
    }
}
void Unit::updateSelectionVisuals(float dt) {
    // Visual feedback for selected units
    if (selectable->selected) {
        // Could add pulsing glow, selection circle, etc.
        float pulse = std::sin(anim->currentTime * 10.0f) * 0.1f + 1.0f;
        sprite->setScale(sf::Vector2f(pulse, pulse));
    } else {
        sprite->setScale(sf::Vector2f(1.0f, 1.0f));
    }
}