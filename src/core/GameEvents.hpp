#pragma once
#include <string>
#include <SFML/System/Vector2.hpp>
#include <memory>
// Forward declarations
class Enemy;
class Tower;
class Unit;
// Event Topics
namespace Events {
    // Gameplay Events
    const std::string ENEMY_SPAWNED = "enemy_spawned";
    const std::string ENEMY_DIED = "enemy_died";
    const std::string ENEMY_REACHED_END = "enemy_reached_end";
    const std::string TOWER_PLACED = "tower_placed";
    const std::string TOWER_UPGRADED = "tower_upgraded";
    const std::string TOWER_SOLD = "tower_sold";
    const std::string UNIT_SPAWNED = "unit_spawned";
    const std::string UNIT_DIED = "unit_died";
    const std::string UNIT_UPGRADED = "unit_upgraded";
    const std::string PROJECTILE_FIRED = "projectile_fired";
    const std::string PROJECTILE_HIT = "projectile_hit";
    // Wave Events
    const std::string WAVE_STARTED = "wave_started";
    const std::string WAVE_COMPLETED = "wave_completed";
    const std::string ALL_WAVES_COMPLETED = "all_waves_completed";
    // Resource Events
    const std::string GOLD_CHANGED = "gold_changed";
    const std::string LIVES_CHANGED = "lives_changed";
    const std::string SCORE_CHANGED = "score_changed";
    // Game State Events
    const std::string GAME_PAUSED = "game_paused";
    const std::string GAME_RESUMED = "game_resumed";
    const std::string GAME_VICTORY = "game_victory";
    const std::string GAME_DEFEAT = "game_defeat";   
    // UI Events
    const std::string UI_BUTTON_CLICKED = "ui_button_clicked";
    const std::string TOWER_SELECTED = "tower_selected";
    const std::string UNIT_SELECTED = "unit_selected";
}
// Event Data Structures
struct EnemyEventData {
    std::shared_ptr<Enemy> enemy;
    sf::Vector2f position;
    std::string enemyType;
    int goldReward;
};
struct TowerEventData {
    std::shared_ptr<Tower> tower;
    sf::Vector2f position;
    std::string towerType;
    int level;
    int cost;
};
struct UnitEventData {
    std::shared_ptr<Unit> unit;
    sf::Vector2f position;
    std::string unitType;
    int level;
};
struct WaveEventData {
    int waveNumber;
    int totalWaves;
    int enemyCount;
    int goldReward;
};
struct ResourceEventData {
    int amount;
    int previousAmount;
    int changeAmount;
};
struct ProjectileEventData {
    sf::Vector2f position;
    sf::Vector2f direction;
    std::string projectileType;
    int damage;
};