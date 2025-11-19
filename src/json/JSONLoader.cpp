#include "../json/JSONLoader.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
bool JSONLoader::loadAllGameData() {
    std::cout << "[JSONLoader] Loading all game data..." << std::endl;
    bool success = true;
    // Load each JSON file
    success &= loadEnemies("data/enemies.json");
    success &= loadUnits("data/units.json");
    success &= loadTowers("data/towers.json");
    success &= loadLevels("data/waves.json"); // waves are in levels
    std::cout << "[JSONLoader] Game data loading " << (success ? "SUCCESS" : "FAILED") << std::endl;
    std::cout << "[JSONLoader] Loaded: " << enemies_.size() << " enemies, "
              << units_.size() << " units, " << towers_.size() << " towers, "
              << waves_.size() << " waves" << std::endl;   
    return success;
}
bool JSONLoader::loadEnemies(const std::string& file) {
    std::cout << "[JSONLoader] Loading enemies from: " << file << std::endl;
    if (!std::filesystem::exists(file)) {
        std::cerr << "[JSONLoader] Enemy file not found: " << file << std::endl;
        return false;
    }
    try {
        std::ifstream f(file);
        json data = json::parse(f);   
        if (data.contains("enemies") && data["enemies"].is_array()) {
            return parseEnemyData(data["enemies"]);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[JSONLoader] Error parsing enemies JSON: " << e.what() << std::endl;
        return false;
    }   
    return false;
}
bool JSONLoader::loadUnits(const std::string& file) {
    std::cout << "[JSONLoader] Loading units from: " << file << std::endl;
    if (!std::filesystem::exists(file)) {
        std::cerr << "[JSONLoader] Unit file not found: " << file << std::endl;
        return false;
    }
    try {
        std::ifstream f(file);
        json data = json::parse(f);   
        if (data.contains("units") && data["units"].is_array()) {
            return parseUnitData(data["units"]);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[JSONLoader] Error parsing units JSON: " << e.what() << std::endl;
        return false;
    }   
    return false;
}
bool JSONLoader::loadTowers(const std::string& file) {
    std::cout << "[JSONLoader] Loading towers from: " << file << std::endl;
    if (!std::filesystem::exists(file)) {
        std::cerr << "[JSONLoader] Tower file not found: " << file << std::endl;
        return false;
    }
    try {
        std::ifstream f(file);
        json data = json::parse(f);   
        if (data.contains("towers") && data["towers"].is_array()) {
            return parseTowerData(data["towers"]);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[JSONLoader] Error parsing towers JSON: " << e.what() << std::endl;
        return false;
    }   
    return false;
}
bool JSONLoader::loadLevels(const std::string& file) {
    std::cout << "[JSONLoader] Loading levels/waves from: " << file << std::endl;
    if (!std::filesystem::exists(file)) {
        std::cerr << "[JSONLoader] Wave file not found: " << file << std::endl;
        return false;
    }
    try {
        std::ifstream f(file);
        json data = json::parse(f);   
        if (data.contains("waves") && data["waves"].is_array()) {
            return parseWaveData(data["waves"]);
        }
    }
    catch (const std::exception& e) {
        std::cerr << "[JSONLoader] Error parsing waves JSON: " << e.what() << std::endl;
        return false;
    }   
    return false;
}
bool JSONLoader::parseEnemyData(const json& data) {
    int count = 0;
    for (const auto& enemyData : data) {
        try {
            std::string id = enemyData["id"];
            StatsComp stats;
            // Convert health to stats (you might want to adjust this based on your component structure)
            stats.speed = enemyData["speed"];
            stats.damage = 0; // Enemies might not have damage in stats component
            stats.attackRange = 0; // Adjust as needed
            enemies_[id] = stats;
            count++;       
            std::cout << "[JSONLoader] Loaded enemy: " << id 
                      << " (speed: " << stats.speed << ")" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "[JSONLoader] Error parsing enemy: " << e.what() << std::endl;
        }
    }   
    std::cout << "[JSONLoader] Successfully loaded " << count << " enemies" << std::endl;
    return count > 0;
}
bool JSONLoader::parseUnitData(const json& data) {
    int count = 0;
    for (const auto& unitData : data) {
        try {
            std::string id = unitData["id"];
            StatsComp stats;
            stats.speed = unitData["speed"];
            stats.damage = unitData["damage"];
            stats.attackRange = unitData["attack_range"];
            stats.attackSpeed = unitData["attack_speed"];
            units_[id] = stats;
            count++;       
            std::cout << "[JSONLoader] Loaded unit: " << id 
                      << " (damage: " << stats.damage << ", range: " << stats.attackRange << ")" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "[JSONLoader] Error parsing unit: " << e.what() << std::endl;
        }
    }   
    std::cout << "[JSONLoader] Successfully loaded " << count << " units" << std::endl;
    return count > 0;
}
bool JSONLoader::parseTowerData(const json& data) {
    int count = 0;
    for (const auto& towerData : data) {
        try {
            std::string id = towerData["id"];
            StatsComp stats;
            stats.damage = towerData["damage"];
            stats.attackRange = towerData["range"];
            stats.attackSpeed = towerData["attack_speed"];
            // Note: towers don't have speed in your JSON
            towers_[id] = stats;
            count++;       
            std::cout << "[JSONLoader] Loaded tower: " << id 
                      << " (damage: " << stats.damage << ", range: " << stats.attackRange << ")" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "[JSONLoader] Error parsing tower: " << e.what() << std::endl;
        }
    }   
    std::cout << "[JSONLoader] Successfully loaded " << count << " towers" << std::endl;
    return count > 0;
}
bool JSONLoader::parseWaveData(const json& data) {
    int count = 0;
    waves_.clear();
    for (const auto& waveData : data) {
        try {
            Wave wave;
            wave.id = waveData["id"];
            if (waveData.contains("groups") && waveData["groups"].is_array()) {
                for (const auto& groupData : waveData["groups"]) {
                    SpawnGroup group;
                    group.id = groupData["enemy_id"];
                    group.count = groupData["count"];
                    group.interval = groupData["interval"];
                    group.delay = groupData["delay"];
                    wave.groups.push_back(group);
                }
            }
            waves_.push_back(wave);
            count++;       
            std::cout << "[JSONLoader] Loaded wave: " << wave.id 
                      << " with " << wave.groups.size() << " groups" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "[JSONLoader] Error parsing wave: " << e.what() << std::endl;
        }
    }   
    std::cout << "[JSONLoader] Successfully loaded " << count << " waves" << std::endl;
    return count > 0;
}
bool JSONLoader::loadAnimations(const std::string& folder) {
    // TODO: Implement animation loading from sprite sheets
    std::cout << "[JSONLoader] Animation loading not yet implemented" << std::endl;
    return true;
}
const StatsComp& JSONLoader::enemyStats(const std::string& id) const {
    auto it = enemies_.find(id);
    if (it != enemies_.end()) return it->second;
    static StatsComp defaultStats;
    return defaultStats;
}
const StatsComp& JSONLoader::unitStats(const std::string& id) const {
    auto it = units_.find(id);
    if (it != units_.end()) return it->second;
    static StatsComp defaultStats;
    return defaultStats;
}
const StatsComp& JSONLoader::towerStats(const std::string& id) const {
    auto it = towers_.find(id);
    if (it != towers_.end()) return it->second;
    static StatsComp defaultStats;
    return defaultStats;
}
const std::vector<Animation>& JSONLoader::getAtlas(const std::string& id) const {
    auto it = atlases_.find(id);
    if (it != atlases_.end()) return it->second;
    static std::vector<Animation> empty;
    return empty;
}
const std::vector<Wave>& JSONLoader::getLevelWaves(int levelId) const {
    auto it = levelWaves_.find(levelId);
    if (it != levelWaves_.end()) return it->second;
    static std::vector<Wave> empty;
    return empty;
}