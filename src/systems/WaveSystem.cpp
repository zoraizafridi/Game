#include "../systems/WaveSystem.hpp"
#include <iostream>

WaveSystem::WaveSystem() 
    : active_(false), currentWaveIndex_(0), spawnTimer_(0.0f), 
      currentGroupIndex_(0), currentGroupCount_(0), allSpawned_(false),
      waitingForDelay_(false) {
}

void WaveSystem::load(const std::vector<Wave>& waves) {
    waves_ = waves;
    std::cout << "[WaveSystem] Loaded " << waves_.size() << " waves" << std::endl;
}

void WaveSystem::start(int waveIndex) {
    if (waveIndex < 0 || waveIndex >= static_cast<int>(waves_.size())) {
        std::cerr << "[WaveSystem] Invalid wave index: " << waveIndex << std::endl;
        return;
    }
    
    currentWaveIndex_ = waveIndex;
    currentGroupIndex_ = 0;
    currentGroupCount_ = 0;
    spawnTimer_ = 0.0f;
    active_ = true;
    allSpawned_ = false;
    waitingForDelay_ = true;
    
    // Set initial delay for first group
    if (!waves_[waveIndex].groups.empty()) {
        spawnTimer_ = -waves_[waveIndex].groups[0].delay; // Negative means we're in delay
    }
    
    std::cout << "[WaveSystem] ========================================" << std::endl;
    std::cout << "[WaveSystem] Starting wave " << (waveIndex + 1) << std::endl;
    std::cout << "[WaveSystem] Groups: " << waves_[waveIndex].groups.size() << std::endl;
    
    for (size_t i = 0; i < waves_[waveIndex].groups.size(); ++i) {
        const auto& group = waves_[waveIndex].groups[i];
        std::cout << "[WaveSystem]   Group " << i << ": " << group.count << "x " << group.id 
                  << " (interval: " << group.interval << "s, delay: " << group.delay << "s)" << std::endl;
    }
    std::cout << "[WaveSystem] ========================================" << std::endl;
}

void WaveSystem::update(float dt) {
    if (!active_) return;
    
    // Check if we've finished spawning all groups
    if (currentGroupIndex_ >= waves_[currentWaveIndex_].groups.size()) {
        if (!allSpawned_) {
            allSpawned_ = true;
            std::cout << "[WaveSystem] ✓ All enemies spawned for wave " << (currentWaveIndex_ + 1) << std::endl;
        }
        return;
    }
    
    const auto& currentGroup = waves_[currentWaveIndex_].groups[currentGroupIndex_];
    
    // Always increment timer
    spawnTimer_ += dt;
    
    // If we're in delay period (negative timer or waiting for delay)
    if (waitingForDelay_) {
        if (spawnTimer_ >= 0.0f) {
            // Delay is over, ready to spawn
            waitingForDelay_ = false;
            spawnTimer_ = 0.0f;
            std::cout << "[WaveSystem] Delay over for group " << currentGroupIndex_ << " - starting spawns" << std::endl;
        }
        return;
    }
    
    // Check if it's time to spawn
    if (spawnTimer_ >= currentGroup.interval) {
        if (currentGroupCount_ < currentGroup.count) {
            if (spawnCallback_) {
                std::cout << "[WaveSystem] Spawning " << currentGroup.id << " (" 
                          << (currentGroupCount_ + 1) << "/" << currentGroup.count << ")" << std::endl;
                spawnCallback_(currentGroup.id);
                currentGroupCount_++;
            }
            spawnTimer_ = 0.0f;
        }
        
        // If we've spawned all in this group, move to next
        if (currentGroupCount_ >= currentGroup.count) {
            std::cout << "[WaveSystem] Group " << currentGroupIndex_ << " complete" << std::endl;
            currentGroupIndex_++;
            currentGroupCount_ = 0;
            spawnTimer_ = 0.0f;
            
            // Check if there's a next group
            if (currentGroupIndex_ < waves_[currentWaveIndex_].groups.size()) {
                // Set up delay for next group
                const auto& nextGroup = waves_[currentWaveIndex_].groups[currentGroupIndex_];
                spawnTimer_ = -nextGroup.delay;
                waitingForDelay_ = true;
                std::cout << "[WaveSystem] Moving to group " << currentGroupIndex_ 
                          << " with " << nextGroup.delay << "s delay" << std::endl;
            }
        }
    }
}

bool WaveSystem::isActive() const {
    return active_;
}

bool WaveSystem::isFinished() const {
    return allSpawned_;
}

void WaveSystem::setSpawnCallback(SpawnCallback callback) {
    spawnCallback_ = callback;
}

int WaveSystem::getCurrentWave() const {
    return currentWaveIndex_;
}

int WaveSystem::getTotalWaves() const {
    return waves_.size();
}