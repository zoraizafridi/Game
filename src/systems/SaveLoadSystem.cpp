#include "../systems/SaveLoadSystem.hpp"
#include "../entities/Tower.hpp"
#include "../entities/Unit.hpp"
#include "../components/Transform.hpp"
#include "../components/Stats.hpp"
#include "../components/Health.hpp"
#include "../components/UpgradeComp.hpp"
#include <fstream>
#include <iostream>
#include <filesystem>
bool SaveLoadSystem::saveGame(const std::string& filename, const GameSaveData& gameData,
                               const std::vector<std::shared_ptr<Tower>>& towers,
                               const std::vector<std::shared_ptr<Unit>>& units) {
    std::cout << "[SaveLoadSystem] Saving game to: " << filename << std::endl;
    try {
        json saveJson;
        // Save game data
        saveJson["gold"] = gameData.gold;
        saveJson["lives"] = gameData.lives;
        saveJson["currentWave"] = gameData.currentWave;
        saveJson["score"] = gameData.score;
        saveJson["nextWaveTimer"] = gameData.nextWaveTimer;
        saveJson["waveInProgress"] = gameData.waveInProgress;
        saveJson["currentMap"] = gameData.currentMap;
        // Save towers
        json towersJson = json::array();
        for (const auto& tower : towers) {
            towersJson.push_back(serializeTower(tower));
        }
        saveJson["towers"] = towersJson;
        // Save units
        json unitsJson = json::array();
        for (const auto& unit : units) {
            unitsJson.push_back(serializeUnit(unit));
        }
        saveJson["units"] = unitsJson;
        // Write to file
        std::ofstream file(filename);
        if (!file.is_open()) {
            std::cerr << "[SaveLoadSystem] Failed to open file for writing: " << filename << std::endl;
            return false;
        }
        file << saveJson.dump(4);
        file.close();
        std::cout << "[SaveLoadSystem] Game saved successfully!" << std::endl;
        return true;       
    } catch (const std::exception& e) {
        std::cerr << "[SaveLoadSystem] Error saving game: " << e.what() << std::endl;
        return false;
    }
}
bool SaveLoadSystem::loadGame(const std::string& filename, GameSaveData& gameData,
                               std::vector<std::shared_ptr<Tower>>& towers,
                               std::vector<std::shared_ptr<Unit>>& units) {
    std::cout << "[SaveLoadSystem] Loading game from: " << filename << std::endl;
    try {
        std::ifstream file(filename);
        if (!file.is_open()) {
            std::cerr << "[SaveLoadSystem] Failed to open file for reading: " << filename << std::endl;
            return false;
        }
        json saveJson = json::parse(file);
        file.close();
        // Load game data
        gameData.gold = saveJson["gold"];
        gameData.lives = saveJson["lives"];
        gameData.currentWave = saveJson["currentWave"];
        gameData.score = saveJson["score"];
        gameData.nextWaveTimer = saveJson["nextWaveTimer"];
        gameData.waveInProgress = saveJson["waveInProgress"];
        gameData.currentMap = saveJson["currentMap"];
        // Load towers
        towers.clear();
        if (saveJson.contains("towers") && saveJson["towers"].is_array()) {
            for (const auto& towerJson : saveJson["towers"]) {
                auto tower = deserializeTower(towerJson);
                if (tower) {
                    towers.push_back(tower);
                }
            }
        }
        // Load units
        units.clear();
        if (saveJson.contains("units") && saveJson["units"].is_array()) {
            for (const auto& unitJson : saveJson["units"]) {
                auto unit = deserializeUnit(unitJson);
                if (unit) {
                    units.push_back(unit);
                }
            }
        }
        std::cout << "[SaveLoadSystem] Game loaded successfully!" << std::endl;
        std::cout << "  Gold: " << gameData.gold << std::endl;
        std::cout << "  Lives: " << gameData.lives << std::endl;
        std::cout << "  Wave: " << gameData.currentWave << std::endl;
        std::cout << "  Towers: " << towers.size() << std::endl;
        std::cout << "  Units: " << units.size() << std::endl;
        return true;       
    } catch (const std::exception& e) {
        std::cerr << "[SaveLoadSystem] Error loading game: " << e.what() << std::endl;
        return false;
    }
}
bool SaveLoadSystem::saveExists(const std::string& filename) {
    return std::filesystem::exists(filename);
}
bool SaveLoadSystem::deleteSave(const std::string& filename) {
    try {
        if (std::filesystem::exists(filename)) {
            std::filesystem::remove(filename);
            std::cout << "[SaveLoadSystem] Save file deleted: " << filename << std::endl;
            return true;
        }
        return false;
    } catch (const std::exception& e) {
        std::cerr << "[SaveLoadSystem] Error deleting save: " << e.what() << std::endl;
        return false;
    }
}
json SaveLoadSystem::serializeTower(const std::shared_ptr<Tower>& tower) {
    json towerJson;
    towerJson["type"] = tower->towerType;
    towerJson["position_x"] = tower->transform->position.x;
    towerJson["position_y"] = tower->transform->position.y;
    towerJson["level"] = tower->upgrade->level;
    towerJson["damage"] = tower->stats->damage;
    towerJson["range"] = tower->stats->attackRange;
    towerJson["attack_speed"] = tower->stats->attackSpeed;   
    return towerJson;
}
json SaveLoadSystem::serializeUnit(const std::shared_ptr<Unit>& unit) {
    json unitJson;
    unitJson["type"] = unit->unitType;
    unitJson["position_x"] = unit->transform->position.x;
    unitJson["position_y"] = unit->transform->position.y;
    unitJson["level"] = unit->upgrade->level;
    unitJson["hp"] = unit->health->hp;
    unitJson["max_hp"] = unit->health->maxHp;
    unitJson["damage"] = unit->stats->damage;
    unitJson["speed"] = unit->stats->speed;
    return unitJson;
}
std::shared_ptr<Tower> SaveLoadSystem::deserializeTower(const json& data) {
    try {
        auto tower = std::make_shared<Tower>();
        tower->initializeAsType(data["type"]);
        tower->transform->position.x = data["position_x"];
        tower->transform->position.y = data["position_y"];
        tower->upgrade->level = data["level"];
        tower->stats->damage = data["damage"];
        tower->stats->attackRange = data["range"];
        tower->stats->attackSpeed = data["attack_speed"];
        return tower;       
    } catch (const std::exception& e) {
        std::cerr << "[SaveLoadSystem] Error deserializing tower: " << e.what() << std::endl;
        return nullptr;
    }
}
std::shared_ptr<Unit> SaveLoadSystem::deserializeUnit(const json& data) {
    try {
        auto unit = std::make_shared<Unit>();
        unit->initializeAsType(data["type"]);
        unit->transform->position.x = data["position_x"];
        unit->transform->position.y = data["position_y"];
        unit->upgrade->level = data["level"];
        unit->health->hp = data["hp"];
        unit->health->maxHp = data["max_hp"];
        unit->stats->damage = data["damage"];
        unit->stats->speed = data["speed"];
        return unit;
    } catch (const std::exception& e) {
        std::cerr << "[SaveLoadSystem] Error deserializing unit: " << e.what() << std::endl;
        return nullptr;
    }
}