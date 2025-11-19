#include "../systems/AnimationAtlasLoader.hpp"
#include <iostream>

void AnimationAtlasLoader::loadAllAtlases(AnimationSystem* animSystem) {
    std::cout << "[AnimationAtlasLoader] Loading all animation atlases..." << std::endl;
    
    loadEnemyAnimations(animSystem);
    loadTowerAnimations(animSystem);
    loadUnitAnimations(animSystem);
    loadProjectileAnimations(animSystem);
    
    std::cout << "[AnimationAtlasLoader] All atlases loaded!" << std::endl;
}

void AnimationAtlasLoader::loadEnemyAnimations(AnimationSystem* animSystem) {
    std::vector<Animation> enemyAnims;
    
    // Idle animation
    Animation idle;
    idle.name = "idle";
    idle.frameTime = 0.15f;
    idle.loop = true;
    idle.frames = {
        sf::IntRect(0, 0, 32, 32),
        sf::IntRect(32, 0, 32, 32),
        sf::IntRect(64, 0, 32, 32),
        sf::IntRect(96, 0, 32, 32)
    };
    enemyAnims.push_back(idle);
    
    // Walk animation
    Animation walk;
    walk.name = "walk";
    walk.frameTime = 0.12f;
    walk.loop = true;
    walk.frames = {
        sf::IntRect(0, 32, 32, 32),
        sf::IntRect(32, 32, 32, 32),
        sf::IntRect(64, 32, 32, 32),
        sf::IntRect(96, 32, 32, 32)
    };
    enemyAnims.push_back(walk);
    
    // Attack animation
    Animation attack;
    attack.name = "attack";
    attack.frameTime = 0.1f;
    attack.loop = false;
    attack.frames = {
        sf::IntRect(0, 64, 32, 32),
        sf::IntRect(32, 64, 32, 32),
        sf::IntRect(64, 64, 32, 32),
        sf::IntRect(96, 64, 32, 32)
    };
    enemyAnims.push_back(attack);
    
    // Death animation
    Animation death;
    death.name = "death";
    death.frameTime = 0.15f;
    death.loop = false;
    death.frames = {
        sf::IntRect(0, 96, 32, 32),
        sf::IntRect(32, 96, 32, 32),
        sf::IntRect(64, 96, 32, 32),
        sf::IntRect(96, 96, 32, 32)
    };
    enemyAnims.push_back(death);
    
    // Register for all enemy types
    animSystem->registerAtlas("goblin", enemyAnims);
    animSystem->registerAtlas("orc", enemyAnims);
    animSystem->registerAtlas("troll", enemyAnims);
    animSystem->registerAtlas("goblin_rider", enemyAnims);
    animSystem->registerAtlas("orc_shaman", enemyAnims);
    animSystem->registerAtlas("wolf_rider", enemyAnims);
    animSystem->registerAtlas("ogre", enemyAnims);
    animSystem->registerAtlas("goblin_archer", enemyAnims);
    animSystem->registerAtlas("dark_knight", enemyAnims);
    animSystem->registerAtlas("dragon_whelp", enemyAnims);
    animSystem->registerAtlas("specter", enemyAnims);
    animSystem->registerAtlas("giant_spider", enemyAnims);
    
    std::cout << "[AnimationAtlasLoader] Enemy animations loaded" << std::endl;
}

void AnimationAtlasLoader::loadTowerAnimations(AnimationSystem* animSystem) {
    std::vector<Animation> towerAnims;
    
    // Idle animation
    Animation idle;
    idle.name = "idle";
    idle.frameTime = 0.2f;
    idle.loop = true;
    idle.frames = {
        sf::IntRect(0, 0, 64, 64)
    };
    towerAnims.push_back(idle);
    
    // Attack animation
    Animation attack;
    attack.name = "attack";
    attack.frameTime = 0.1f;
    attack.loop = false;
    attack.frames = {
        sf::IntRect(0, 0, 64, 64),
        sf::IntRect(64, 0, 64, 64),
        sf::IntRect(128, 0, 64, 64),
        sf::IntRect(64, 0, 64, 64),
        sf::IntRect(0, 0, 64, 64)
    };
    towerAnims.push_back(attack);
    
    // Build animation
    Animation build;
    build.name = "build";
    build.frameTime = 0.15f;
    build.loop = false;
    build.frames = {
        sf::IntRect(0, 64, 64, 64),
        sf::IntRect(64, 64, 64, 64),
        sf::IntRect(128, 64, 64, 64),
        sf::IntRect(0, 0, 64, 64)
    };
    towerAnims.push_back(build);
    
    // Register for all tower types
    animSystem->registerAtlas("arrow_tower", towerAnims);
    animSystem->registerAtlas("cannon_tower", towerAnims);
    animSystem->registerAtlas("mage_tower", towerAnims);
    animSystem->registerAtlas("ice_tower", towerAnims);
    animSystem->registerAtlas("lightning_tower", towerAnims);
    animSystem->registerAtlas("poison_tower", towerAnims);
    animSystem->registerAtlas("ballista", towerAnims);
    animSystem->registerAtlas("flame_tower", towerAnims);
    animSystem->registerAtlas("tesla_tower", towerAnims);
    animSystem->registerAtlas("arcane_tower", towerAnims);
    animSystem->registerAtlas("sniper_tower", towerAnims);
    animSystem->registerAtlas("artillery_tower", towerAnims);
    
    std::cout << "[AnimationAtlasLoader] Tower animations loaded" << std::endl;
}

void AnimationAtlasLoader::loadUnitAnimations(AnimationSystem* animSystem) {
    std::vector<Animation> unitAnims;
    
    // Idle
    Animation idle;
    idle.name = "idle";
    idle.frameTime = 0.15f;
    idle.loop = true;
    idle.frames = {
        sf::IntRect(0, 0, 32, 32),
        sf::IntRect(32, 0, 32, 32),
        sf::IntRect(64, 0, 32, 32),
        sf::IntRect(96, 0, 32, 32)
    };
    unitAnims.push_back(idle);
    
    // Walk
    Animation walk;
    walk.name = "walk";
    walk.frameTime = 0.12f;
    walk.loop = true;
    walk.frames = {
        sf::IntRect(0, 32, 32, 32),
        sf::IntRect(32, 32, 32, 32),
        sf::IntRect(64, 32, 32, 32),
        sf::IntRect(96, 32, 32, 32)
    };
    unitAnims.push_back(walk);
    
    // Attack
    Animation attack;
    attack.name = "attack";
    attack.frameTime = 0.1f;
    attack.loop = false;
    attack.frames = {
        sf::IntRect(0, 64, 32, 32),
        sf::IntRect(32, 64, 32, 32),
        sf::IntRect(64, 64, 32, 32),
        sf::IntRect(96, 64, 32, 32)
    };
    unitAnims.push_back(attack);
    
    // Death
    Animation death;
    death.name = "death";
    death.frameTime = 0.15f;
    death.loop = false;
    death.frames = {
        sf::IntRect(0, 96, 32, 32),
        sf::IntRect(32, 96, 32, 32),
        sf::IntRect(64, 96, 32, 32),
        sf::IntRect(96, 96, 32, 32)
    };
    unitAnims.push_back(death);
    
    // Register for all unit types
    animSystem->registerAtlas("archer", unitAnims);
    animSystem->registerAtlas("knight", unitAnims);
    animSystem->registerAtlas("mage", unitAnims);
    animSystem->registerAtlas("rogue", unitAnims);
    animSystem->registerAtlas("paladin", unitAnims);
    animSystem->registerAtlas("ranger", unitAnims);
    animSystem->registerAtlas("berserker", unitAnims);
    animSystem->registerAtlas("priest", unitAnims);
    animSystem->registerAtlas("necromancer", unitAnims);
    animSystem->registerAtlas("druid", unitAnims);
    animSystem->registerAtlas("engineer", unitAnims);
    animSystem->registerAtlas("monk", unitAnims);
    
    std::cout << "[AnimationAtlasLoader] Unit animations loaded" << std::endl;
}

void AnimationAtlasLoader::loadProjectileAnimations(AnimationSystem* animSystem) {
    std::vector<Animation> projectileAnims;
    
    Animation idle;
    idle.name = "idle";
    idle.frameTime = 0.1f;
    idle.loop = true;
    idle.frames = {
        sf::IntRect(0, 0, 16, 16)
    };
    projectileAnims.push_back(idle);
    
    animSystem->registerAtlas("projectiles", projectileAnims);
    
    std::cout << "[AnimationAtlasLoader] Projectile animations loaded" << std::endl;
}