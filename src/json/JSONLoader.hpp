#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include "../components/Stats.hpp"
#include "../json/types.hpp"
#include "../json/json.hpp"  
#include "../systems/AnimationSystem.hpp"
#include <SFML/Graphics.hpp>

using json = nlohmann::json;  

struct Wave;

class JSONLoader {
public:
    bool loadEnemies(const std::string& file);
    bool loadUnits(const std::string& file);
    bool loadTowers(const std::string& file);
    bool loadMaps(const std::string& file);
    bool loadAnimations(const std::string& folder);
    bool loadLevels(const std::string& file);
    
    // Load all JSON data at once
    bool loadAllGameData();
    
    const StatsComp& enemyStats(const std::string& id) const;
    const StatsComp& unitStats(const std::string& id) const;
    const StatsComp& towerStats(const std::string& id) const;
    const std::vector<Animation>& getAtlas(const std::string& id) const;
    const std::vector<Wave>& getLevelWaves(int levelId) const;
    
    // Getters for loaded data
    const std::unordered_map<std::string, StatsComp>& getAllEnemyStats() const { return enemies_; }
    const std::unordered_map<std::string, StatsComp>& getAllUnitStats() const { return units_; }
    const std::unordered_map<std::string, StatsComp>& getAllTowerStats() const { return towers_; }
    const std::vector<Wave>& getAllWaves() const { return waves_; }
    
private:
    std::unordered_map<std::string, StatsComp> enemies_;
    std::unordered_map<std::string, StatsComp> units_;
    std::unordered_map<std::string, StatsComp> towers_;
    std::unordered_map<std::string, std::vector<Animation>> atlases_;
    std::unordered_map<int, std::vector<Wave>> levelWaves_;
    std::vector<Wave> waves_;
    
    // Helper methods for JSON parsing
    bool parseEnemyData(const json& data);
    bool parseUnitData(const json& data);
    bool parseTowerData(const json& data);
    bool parseMapData(const json& data);
    bool parseWaveData(const json& data);
};