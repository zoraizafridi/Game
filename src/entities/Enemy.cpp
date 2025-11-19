#include "../entities/Enemy.hpp"
#include <iostream>

Enemy::Enemy() {
    transform = std::make_shared<Transform>();
    sprite = std::make_shared<SpriteComp>();
    anim = std::make_shared<AnimationStateComp>();
    health = std::make_shared<HealthComp>();
    stats = std::make_shared<StatsComp>();
    path = std::make_shared<PathFollower>();
    collider = std::make_shared<ColliderComp>();
    effects = std::make_shared<StatusEffectsComp>();
    ai = std::make_shared<EnemyAI>();
    
    velocity = sf::Vector2f(0, 0);
    acceleration = sf::Vector2f(0, 0);
    enemyType = "goblin";
}

void Enemy::initialize() {
    health->hp = 100;
    health->maxHp = 100;
    stats->speed = 50.0f;
    stats->damage = 10.0f;
    stats->attackRange = 40.0f;
    stats->attackSpeed = 1.0f;
    collider->radius = 12.0f;
    ai->pathSpeed = 50.0f;
    ai->attackRange = 40.0f;
    ai->attackCooldown = 1.0f;
    
    anim->playing = true;
    anim->currentAnim = "idle";
    anim->atlasId = "enemies";
    anim->frameCount = 4;
    anim->frameDuration = 0.1f;
}

void Enemy::initializeAsType(const std::string& type) {
    enemyType = type;
    
    std::cout << "[Enemy] Initializing as type: " << type << std::endl;
    
    if (type == "goblin") {
        health->hp = 80;
        health->maxHp = 80;
        stats->speed = 60.0f;
        stats->damage = 8.0f;
        ai->pathSpeed = 60.0f;
        sprite->textureId = "enemy_goblin";
        anim->atlasId = "goblin";
    } else if (type == "orc") {
        health->hp = 200;
        health->maxHp = 200;
        stats->speed = 45.0f;
        stats->damage = 15.0f;
        ai->pathSpeed = 45.0f;
        sprite->textureId = "enemy_orc";
        anim->atlasId = "orc";
    } else if (type == "troll") {
        health->hp = 500;
        health->maxHp = 500;
        stats->speed = 30.0f;
        stats->damage = 25.0f;
        ai->pathSpeed = 30.0f;
        sprite->textureId = "enemy_troll";
        anim->atlasId = "troll";
    } else if (type == "goblin_rider") {
        health->hp = 120;
        health->maxHp = 120;
        stats->speed = 80.0f;
        stats->damage = 12.0f;
        ai->pathSpeed = 80.0f;
        sprite->textureId = "enemy_goblin_rider";
        anim->atlasId = "goblin_rider";
    } else if (type == "orc_shaman") {
        health->hp = 150;
        health->maxHp = 150;
        stats->speed = 40.0f;
        stats->damage = 20.0f;
        ai->pathSpeed = 40.0f;
        sprite->textureId = "enemy_orc_shaman";
        anim->atlasId = "orc_shaman";
    } else if (type == "wolf_rider") {
        health->hp = 180;
        health->maxHp = 180;
        stats->speed = 90.0f;
        stats->damage = 18.0f;
        ai->pathSpeed = 90.0f;
        sprite->textureId = "enemy_wolf_rider";
        anim->atlasId = "wolf_rider";
    } else if (type == "ogre") {
        health->hp = 800;
        health->maxHp = 800;
        stats->speed = 25.0f;
        stats->damage = 40.0f;
        ai->pathSpeed = 25.0f;
        sprite->textureId = "enemy_ogre";
        anim->atlasId = "ogre";
    } else if (type == "goblin_archer") {
        health->hp = 70;
        health->maxHp = 70;
        stats->speed = 55.0f;
        stats->damage = 10.0f;
        ai->pathSpeed = 55.0f;
        sprite->textureId = "enemy_goblin_archer";
        anim->atlasId = "goblin_archer";
    } else if (type == "dark_knight") {
        health->hp = 400;
        health->maxHp = 400;
        stats->speed = 35.0f;
        stats->damage = 35.0f;
        ai->pathSpeed = 35.0f;
        sprite->textureId = "enemy_dark_knight";
        anim->atlasId = "dark_knight";
    } else if (type == "dragon_whelp") {
        health->hp = 300;
        health->maxHp = 300;
        stats->speed = 70.0f;
        stats->damage = 30.0f;
        ai->pathSpeed = 70.0f;
        sprite->textureId = "enemy_dragon_whelp";
        anim->atlasId = "dragon_whelp";
    } else if (type == "specter") {
        health->hp = 250;
        health->maxHp = 250;
        stats->speed = 50.0f;
        stats->damage = 25.0f;
        ai->pathSpeed = 50.0f;
        sprite->textureId = "enemy_specter";
        anim->atlasId = "specter";
    } else if (type == "giant_spider") {
        health->hp = 180;
        health->maxHp = 180;
        stats->speed = 65.0f;
        stats->damage = 20.0f;
        ai->pathSpeed = 65.0f;
        sprite->textureId = "enemy_giant_spider";
        anim->atlasId = "giant_spider";
    }
    
    // Ensure path follower has proper settings
    path->speed = ai->pathSpeed;
    path->arrivalThreshold = 10.0f;  // Increased from 5.0f to prevent getting stuck
    path->loop = false;
    path->finished = false;
    path->currentIndex = 0;
    
    std::cout << "[Enemy] Initialized " << type << " - HP: " << health->hp << ", Speed: " << ai->pathSpeed << std::endl;
}

void Enemy::update(float dt) {
    if (isDead) {
        updateDeath(dt);
        return;
    }
    
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
    bool wasMoving = isMoving;
    bool wasAttacking = isAttacking;
    isMoving = false;
    isAttacking = false;
    
    // Update systems
    updateMovement(dt);
    updateCombat(dt);
    updateStatusEffects(dt);
    updateAnimation(dt);
    
    // Update AI cooldown
    if (ai->cooldown > 0) {
        ai->cooldown -= dt;
    }
    
    // Update attack timer
    if (attackTimer > 0) {
        attackTimer -= dt;
        if (attackTimer <= 0) {
            isAttacking = false;
        }
    }
}

void Enemy::updateMovement(float dt) {
    if (path->hasPath() && !path->finished) {
        sf::Vector2f target = path->getCurrentTarget();
        sf::Vector2f direction = target - transform->position;
        float distance = std::sqrt(direction.x * direction.x + direction.y * direction.y);
        
        if (distance < path->arrivalThreshold) {
            path->currentIndex++;
            if (path->currentIndex >= path->path.size()) {
                path->finished = true;
                velocity = sf::Vector2f(0, 0);
            }
        } else {
            // Smooth acceleration-based movement
            sf::Vector2f desiredVelocity = (direction / distance) * ai->pathSpeed;
            sf::Vector2f steering = desiredVelocity - velocity;
            acceleration = steering * 8.0f;
            velocity += acceleration * dt;
            
            // Limit velocity
            float speed = std::sqrt(velocity.x * velocity.x + velocity.y * velocity.y);
            if (speed > ai->pathSpeed) {
                velocity = (velocity / speed) * ai->pathSpeed;
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

void Enemy::updateCombat(float dt) {
    if (ai->attackRange > 0 && ai->cooldown <= 0) {
        if (attackTimer <= 0) {
            isAttacking = true;
            attackTimer = 0.5f;
            ai->cooldown = ai->attackCooldown;
        }
    }
}

void Enemy::updateAnimation(float dt) {
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

void Enemy::updateDeath(float dt) {
    deathTimer -= dt;
}

void Enemy::updateStatusEffects(float dt) {
    if (!effects->list.empty()) {
        for (const auto& effect : effects->list) {
            switch (effect.type) {
                case 0: // Slow
                    break;
                case 1: // Burn
                    break;
                case 2: // Stun
                    break;
                case 3: // Poison
                    break;
            }
        }
    }
}