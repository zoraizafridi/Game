#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../json/json.hpp"
using json = nlohmann::json;
class Tower;
class Unit;
class Enemy;
struct GameSaveData {
    int gold;
    int lives;
    int currentWave;
    int score;
    float nextWaveTimer;
    bool waveInProgress;
    std::string currentMap;
};
class SaveLoadSystem {
public:
    static bool saveGame(const std::string& filename, const GameSaveData& gameData,
                        const std::vector<std::shared_ptr<Tower>>& towers,
                        const std::vector<std::shared_ptr<Unit>>& units);
    
    static bool loadGame(const std::string& filename, GameSaveData& gameData,
                        std::vector<std::shared_ptr<Tower>>& towers,
                        std::vector<std::shared_ptr<Unit>>& units);
    
    static bool saveExists(const std::string& filename);
    static bool deleteSave(const std::string& filename);
    
private:
    static json serializeTower(const std::shared_ptr<Tower>& tower);
    static json serializeUnit(const std::shared_ptr<Unit>& unit);
    
    static std::shared_ptr<Tower> deserializeTower(const json& data);
    static std::shared_ptr<Unit> deserializeUnit(const json& data);
};