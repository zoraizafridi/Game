#pragma once
#include <vector>
#include <functional>
#include <string>
#include "../json/types.hpp"

class WaveSystem {
public:
    using SpawnCallback = std::function<void(const std::string& enemyId)>;
    
    WaveSystem();
    
    void load(const std::vector<Wave>& waves);
    void start(int waveIndex);
    void update(float dt);
    
    bool isActive() const;
    bool isFinished() const;
    
    void setSpawnCallback(SpawnCallback callback);
    
    int getCurrentWave() const;
    int getTotalWaves() const;

private:
    std::vector<Wave> waves_;
    bool active_;
    bool allSpawned_;
    int currentWaveIndex_;
    int currentGroupIndex_;
    int currentGroupCount_;
    float spawnTimer_;
    bool waitingForDelay_;  // Added this member variable
    SpawnCallback spawnCallback_;
};