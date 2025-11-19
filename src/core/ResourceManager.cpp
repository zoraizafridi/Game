#include "../core/ResourceManager.hpp"
#include <iostream>
#include <filesystem>

bool ResourceManager::loadTexture(const std::string& id, const std::string& file) {
    // Check if file exists
    if (!std::filesystem::exists(file)) {
        std::cerr << "[ResourceManager] File not found: " << file << std::endl;
        return false;
    }   
    
    sf::Texture texture;
    if (!texture.loadFromFile(file)) {
        std::cerr << "[ResourceManager] Failed to load texture: " << file << std::endl;
        return false;
    }
    
    textures_[id] = texture;
    
    // Add texture size info for debugging
    sf::Vector2u size = texture.getSize();
    std::cout << "[ResourceManager] Loaded texture: " << id 
              << " from " << file 
              << " (" << size.x << "x" << size.y << ")" << std::endl;
    return true;
}

bool ResourceManager::loadFont(const std::string& id, const std::string& file) {
    if (!std::filesystem::exists(file)) {
        std::cerr << "[ResourceManager] File not found: " << file << std::endl;
        return false;
    }   
    
    sf::Font font;
    if (!font.loadFromFile(file)) {
        std::cerr << "[ResourceManager] Failed to load font: " << file << std::endl;
        return false;
    }
    
    fonts_[id] = font;
    std::cout << "[ResourceManager] Loaded font: " << id << " from " << file << std::endl;
    return true;
}

bool ResourceManager::loadAllAssets() {
    std::cout << "[ResourceManager] Loading all game assets..." << std::endl;
    bool success = true;
    
    // Load all fonts
    success &= loadAllFonts();
    
    // Load all textures by category
    success &= loadEnemyTextures();
    success &= loadTowerTextures();
    success &= loadUnitTextures();
    success &= loadProjectileTextures();
    success &= loadMapTextures();
    success &= loadUITextures();   
    
    std::cout << "[ResourceManager] Asset loading " << (success ? "SUCCESS" : "FAILED") << std::endl;
    std::cout << "[ResourceManager] Loaded " << textures_.size() << " textures and " << fonts_.size() << " fonts" << std::endl;
    
    return success;
}

bool ResourceManager::loadAllFonts() {
    bool success = true;
    success &= loadFont("kenney_future", "data/assets/fonts/Kenney Future.ttf");
    success &= loadFont("kenney_mini", "data/assets/fonts/Kenney Mini.ttf");
    success &= loadFont("kenney_pixel", "data/assets/fonts/Kenney Pixel.ttf");
    return success;
}

bool ResourceManager::loadEnemyTextures() {
    bool success = true;
    success &= loadTexture("enemy_goblin", "data/assets/textures/enemies/goblin.png");
    success &= loadTexture("enemy_orc", "data/assets/textures/enemies/orc.png");
    success &= loadTexture("enemy_troll", "data/assets/textures/enemies/cave_troll.png");
    success &= loadTexture("enemy_goblin_rider", "data/assets/textures/enemies/goblin_rider.png");
    success &= loadTexture("enemy_orc_shaman", "data/assets/textures/enemies/orc_shaman.png");
    success &= loadTexture("enemy_wolf_rider", "data/assets/textures/enemies/wolf_rider.png");
    success &= loadTexture("enemy_ogre", "data/assets/textures/enemies/ogre.png");
    success &= loadTexture("enemy_goblin_archer", "data/assets/textures/enemies/goblin_archer.png");
    success &= loadTexture("enemy_dark_knight", "data/assets/textures/enemies/dark_knight.png");
    success &= loadTexture("enemy_dragon_whelp", "data/assets/textures/enemies/dragon.png");
    success &= loadTexture("enemy_specter", "data/assets/textures/enemies/ancient_specter.png");
    success &= loadTexture("enemy_giant_spider", "data/assets/textures/enemies/ancient_spider.png");
    return success;
}

bool ResourceManager::loadTowerTextures() {
    bool success = true;
    success &= loadTexture("tower_arrow", "data/assets/textures/towers/tower_03.png");
    success &= loadTexture("tower_cannon", "data/assets/textures/towers/tower_04.png");
    success &= loadTexture("tower_mage", "data/assets/textures/towers/tower_10.png");
    success &= loadTexture("tower_ice", "data/assets/textures/towers/tower_16.png");
    success &= loadTexture("tower_lightning", "data/assets/textures/towers/tower_18.png");
    success &= loadTexture("tower_poison", "data/assets/textures/towers/tower_19.png");
    success &= loadTexture("tower_ballista", "data/assets/textures/towers/tower_23.png");
    success &= loadTexture("tower_flame", "data/assets/textures/towers/tower_24.png");
    success &= loadTexture("tower_tesla", "data/assets/textures/towers/tower_25.png");
    success &= loadTexture("tower_arcane", "data/assets/textures/towers/tower_30.png");
    success &= loadTexture("tower_sniper", "data/assets/textures/towers/tower_39.png");
    success &= loadTexture("tower_artillery", "data/assets/textures/towers/tower_50.png");
    return success;
}

bool ResourceManager::loadUnitTextures() {
    bool success = true;
    success &= loadTexture("unit_archer", "data/assets/textures/units/eleven_archer.png");
    success &= loadTexture("unit_knight", "data/assets/textures/units/royal_knight.png");
    success &= loadTexture("unit_mage", "data/assets/textures/units/battle_mage.png");
    success &= loadTexture("unit_rogue", "data/assets/textures/units/shadow_rogue.png");
    success &= loadTexture("unit_paladin", "data/assets/textures/units/holy_paladin.png");
    success &= loadTexture("unit_ranger", "data/assets/textures/units/forest_ranger.png");
    success &= loadTexture("unit_berserker", "data/assets/textures/units/mountain_berserker.png");
    success &= loadTexture("unit_priest", "data/assets/textures/units/divine_priest.png");
    
    // These files don't exist - use placeholder textures instead
    // success &= loadTexture("unit_necromancer", "data/assets/textures/units/dark_necromancer.png");
    // success &= loadTexture("unit_druid", "data/assets/textures/units/ancient_druid.png");
    
    success &= loadTexture("unit_engineer", "data/assets/textures/units/battle_engineer.png");
    success &= loadTexture("unit_monk", "data/assets/textures/units/martial_monk.png");
    return success;
}

bool ResourceManager::loadProjectileTextures() {
    bool success = true;
    success &= loadTexture("projectile_arrow", "data/assets/textures/projectiles/arrow.png");
    success &= loadTexture("projectile_cannonball", "data/assets/textures/projectiles/cannonball.png");
    success &= loadTexture("projectile_fireball", "data/assets/textures/projectiles/fireball.png");
    success &= loadTexture("projectile_ice_shard", "data/assets/textures/projectiles/ice_shard.png");
    success &= loadTexture("projectile_lightning", "data/assets/textures/projectiles/lightning.png");
    success &= loadTexture("projectile_poison_dart", "data/assets/textures/projectiles/poison_dart.png");
    success &= loadTexture("projectile_ballista_bolt", "data/assets/textures/projectiles/ballista_bolt.png");
    success &= loadTexture("projectile_fireball_large", "data/assets/textures/projectiles/fireball_large.png");
    success &= loadTexture("projectile_electric_arc", "data/assets/textures/projectiles/electirc_arc.png");
    success &= loadTexture("projectile_arcane_orb", "data/assets/textures/projectiles/arcane_orb.png");
    success &= loadTexture("projectile_sniper_round", "data/assets/textures/projectiles/sniper_round.png");
    success &= loadTexture("projectile_artillery_shell", "data/assets/textures/projectiles/artillery_shell.png");
    success &= loadTexture("projectile_bolt", "data/assets/textures/projectiles/bolt.png");
    success &= loadTexture("projectile_holy_light", "data/assets/textures/projectiles/holy_light.png");
    success &= loadTexture("projectile_shadow_bolt", "data/assets/textures/projectiles/shadow_bolt.png");
    success &= loadTexture("projectile_nature_blast", "data/assets/textures/projectiles/nature_blast.png");
    return success;
}

bool ResourceManager::loadMapTextures() {
    bool success = true;
    success &= loadTexture("map_forest_path", "data/assets/textures/maps/forest_path.png");
    success &= loadTexture("map_mountain_pass", "data/assets/textures/maps/mountain_pass.png");
    success &= loadTexture("map_desert_oasis", "data/assets/textures/maps/desert_oasis.png");
    success &= loadTexture("map_ice_cavern", "data/assets/textures/maps/ice_cavern.png");
    success &= loadTexture("map_volcano_lair", "data/assets/textures/maps/volcano_lair.png");
    success &= loadTexture("map_castle_ruins", "data/assets/textures/maps/castle_ruins.png");
    success &= loadTexture("map_swamp_marsh", "data/assets/textures/maps/swamp_marsh.png");
    success &= loadTexture("map_crystal_mines", "data/assets/textures/maps/crystal_mines.png");
    success &= loadTexture("map_ancient_temple", "data/assets/textures/maps/ancient_temple.png");
    success &= loadTexture("map_dragon_peak", "data/assets/textures/maps/dragons_peak.png"); // Fixed: file has 's', ID doesn't
    return success;
}

bool ResourceManager::loadUITextures() {
    bool success = true;
    success &= loadTexture("ui_ability", "data/assets/textures/ui/ability.png");
    success &= loadTexture("ui_boss_healthbar", "data/assets/textures/ui/boss_healthbar.png");
    success &= loadTexture("ui_enemy_healthbar", "data/assets/textures/ui/enemy_healthbar.png");
    success &= loadTexture("ui_nextwave", "data/assets/textures/ui/nextwave.png");
    success &= loadTexture("ui_pause_button", "data/assets/textures/ui/pause_button.png");
    success &= loadTexture("ui_resume_button", "data/assets/textures/ui/resume_button.png");
    success &= loadTexture("ui_tower_healthbar", "data/assets/textures/ui/tower_healthbar.png");
    success &= loadTexture("ui_unit_healthbar", "data/assets/textures/ui/unit_healthbar.png");
    return success;
}

const sf::Texture& ResourceManager::getTexture(const std::string& id) const {
    auto it = textures_.find(id);
    if (it == textures_.end()) {
        std::cerr << "[ResourceManager] Texture not found: " << id << std::endl;
        static sf::Texture empty;
        return empty;
    }
    return it->second;
}

const sf::Font& ResourceManager::getFont(const std::string& id) const {
    auto it = fonts_.find(id);
    if (it == fonts_.end()) {
        std::cerr << "[ResourceManager] Font not found: " << id << std::endl;
        static sf::Font empty;
        return empty;
    }
    return it->second;
}

bool ResourceManager::hasTexture(const std::string& id) const {
    return textures_.find(id) != textures_.end();
}

bool ResourceManager::hasFont(const std::string& id) const {
    return fonts_.find(id) != fonts_.end();
}

void ResourceManager::clear() {
    textures_.clear();
    fonts_.clear();
    std::cout << "[ResourceManager] Cleared all resources" << std::endl;
}