#pragma once
#include <memory>
#include <functional>
#include <string>
// Forward declarations
class Tower;
class Unit;
class UpgradeSystem {
public:
    static bool upgradeTower(std::shared_ptr<Tower> tower, int& gold);
    static bool upgradeUnit(std::shared_ptr<Unit> unit, int& gold);
    static int getUpgradeCost(int currentLevel, const std::string& type);
    static bool canUpgrade(int currentLevel, int maxLevel, int gold, const std::string& type);
private:
    static void applyTowerUpgrade(std::shared_ptr<Tower> tower);
    static void applyUnitUpgrade(std::shared_ptr<Unit> unit);
};