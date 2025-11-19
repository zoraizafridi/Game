#include "../systems/UpgradeSystem.hpp"
#include "../entities/Tower.hpp"
#include "../entities/Unit.hpp"
#include "../components/UpgradeComp.hpp"
#include "../components/Stats.hpp"
#include "../components/Health.hpp"
#include <iostream>
bool UpgradeSystem::upgradeTower(std::shared_ptr<Tower> tower, int& gold) {
    if (!tower || !tower->upgrade) {
        std::cout << "[UpgradeSystem] Invalid tower for upgrade" << std::endl;
        return false;
    }
    if (tower->upgrade->level >= tower->upgrade->maxLevel) {
        std::cout << "[UpgradeSystem] Tower already at max level" << std::endl;
        return false;
    }
    int cost = getUpgradeCost(tower->upgrade->level, tower->towerType);
    if (gold < cost) {
        std::cout << "[UpgradeSystem] Not enough gold. Need: " << cost << ", Have: " << gold << std::endl;
        return false;
    }
    gold -= cost;
    tower->upgrade->level++;
    std::cout << "[UpgradeSystem] Upgraded " << tower->towerType 
              << " to level " << tower->upgrade->level << std::endl;
    applyTowerUpgrade(tower);   
    return true;
}
bool UpgradeSystem::upgradeUnit(std::shared_ptr<Unit> unit, int& gold) {
    if (!unit || !unit->upgrade) {
        std::cout << "[UpgradeSystem] Invalid unit for upgrade" << std::endl;
        return false;
    }
    if (unit->upgrade->level >= unit->upgrade->maxLevel) {
        std::cout << "[UpgradeSystem] Unit already at max level" << std::endl;
        return false;
    }
    int cost = getUpgradeCost(unit->upgrade->level, unit->unitType);
    if (gold < cost) {
        std::cout << "[UpgradeSystem] Not enough gold. Need: " << cost << ", Have: " << gold << std::endl;
        return false;
    }
    gold -= cost;
    unit->upgrade->level++;
    std::cout << "[UpgradeSystem] Upgraded " << unit->unitType 
              << " to level " << unit->upgrade->level << std::endl;
    applyUnitUpgrade(unit);   
    return true;
}
int UpgradeSystem::getUpgradeCost(int currentLevel, const std::string& type) {
    int baseCost = 100;
    // Tower costs
    if (type == "arrow_tower") baseCost = 150;
    else if (type == "cannon_tower") baseCost = 300;
    else if (type == "mage_tower") baseCost = 400;
    else if (type == "ice_tower") baseCost = 350;
    else if (type == "lightning_tower") baseCost = 600;
    else if (type == "poison_tower") baseCost = 500;
    else if (type == "ballista") baseCost = 700;
    else if (type == "flame_tower") baseCost = 750;
    else if (type == "tesla_tower") baseCost = 900;
    else if (type == "arcane_tower") baseCost = 1200;
    else if (type == "sniper_tower") baseCost = 1000;
    else if (type == "artillery_tower") baseCost = 1500;
    // Unit costs
    else if (type == "archer") baseCost = 120;
    else if (type == "knight") baseCost = 180;
    else if (type == "mage") baseCost = 220;
    else if (type == "rogue") baseCost = 150;
    else if (type == "paladin") baseCost = 270;
    else if (type == "ranger") baseCost = 200;
    else if (type == "berserker") baseCost = 240;
    else if (type == "priest") baseCost = 210;
    else if (type == "necromancer") baseCost = 255;
    else if (type == "druid") baseCost = 285;
    else if (type == "engineer") baseCost = 315;
    else if (type == "monk") baseCost = 330;   
    // Scale cost by level
    return baseCost * (currentLevel + 1);
}
bool UpgradeSystem::canUpgrade(int currentLevel, int maxLevel, int gold, const std::string& type) {
    if (currentLevel >= maxLevel) return false;
    int cost = getUpgradeCost(currentLevel, type);
    return gold >= cost;
}
void UpgradeSystem::applyTowerUpgrade(std::shared_ptr<Tower> tower) {
    if (!tower || !tower->stats) return;
    int level = tower->upgrade->level;
    // Base improvements per level
    if (level == 2) {
        tower->stats->damage *= 1.5f;
        tower->stats->attackRange *= 1.1f;
        tower->stats->attackSpeed *= 1.2f;
    } else if (level == 3) {
        tower->stats->damage *= 1.7f;
        tower->stats->attackRange *= 1.2f;
        tower->stats->attackSpeed *= 1.3f;
    }   
    std::cout << "[UpgradeSystem] Tower stats - Damage: " << tower->stats->damage 
              << ", Range: " << tower->stats->attackRange 
              << ", Speed: " << tower->stats->attackSpeed << std::endl;
}
void UpgradeSystem::applyUnitUpgrade(std::shared_ptr<Unit> unit) {
    if (!unit || !unit->stats || !unit->health) return;
    int level = unit->upgrade->level;
    // Base improvements per level
    if (level == 2) {
        unit->stats->damage *= 1.5f;
        unit->health->maxHp = static_cast<int>(unit->health->maxHp * 1.3f);
        unit->health->hp = unit->health->maxHp;
        unit->stats->attackSpeed *= 1.2f;
        unit->stats->speed *= 1.1f;
    } else if (level == 3) {
        unit->stats->damage *= 1.7f;
        unit->health->maxHp = static_cast<int>(unit->health->maxHp * 1.5f);
        unit->health->hp = unit->health->maxHp;
        unit->stats->attackSpeed *= 1.3f;
        unit->stats->speed *= 1.15f;
    }
    std::cout << "[UpgradeSystem] Unit stats - HP: " << unit->health->hp 
              << ", Damage: " << unit->stats->damage 
              << ", Speed: " << unit->stats->speed << std::endl;
}