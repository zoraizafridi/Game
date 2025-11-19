#pragma once
#include <vector>
#include <SFML/Graphics/Color.hpp>
struct AppliedEffect {
    int type;       // 0=slow, 1=burn, 2=stun, 3=poison
    float value;    // Effect strength (slow %, burn dps, etc.)
    float remaining;// Time remaining
    float duration; // Total duration  
    // PHASE 3: Visual properties
    sf::Color effectColor;
    float particleTimer = 0.0f;
    float particleInterval = 0.1f;
    bool isVisible = true;
};
struct StatusEffectsComp {
    std::vector<AppliedEffect> list;
    // PHASE 3: Helper methods
    bool hasEffect(int effectType) const {
        for (const auto& effect : list) {
            if (effect.type == effectType) return true;
        }
        return false;
    }
    float getEffectValue(int effectType) const {
        for (const auto& effect : list) {
            if (effect.type == effectType) return effect.value;
        }
        return 0.0f;
    }
    void removeEffect(int effectType) {
        list.erase(std::remove_if(list.begin(), list.end(),
            [effectType](const AppliedEffect& e) { return e.type == effectType; }),
            list.end());
    }
};