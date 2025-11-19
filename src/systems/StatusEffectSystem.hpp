#pragma once
#include <vector>
#include "../entities/Enemy.hpp"
struct StatusEffect {
    int type;     // 0 slow, 1 burn, 2 stun, 3 poison
    float value;
    float time;
    // PHASE 3: Visual properties
    sf::Color color;
    float particleEmitRate = 0.1f;
};
class StatusEffectSystem {
public:
    void apply(Enemy& e, const StatusEffect& s);
    void update(Enemy& e, float dt);
    void updateVisuals(Enemy& e, float dt, class ParticleSystem* particleSystem);
    // PHASE 3: Specific effect applications
    void applySlow(Enemy& e, float strength, float duration);
    void applyBurn(Enemy& e, float dps, float duration);
    void applyStun(Enemy& e, float duration);
    void applyPoison(Enemy& e, float dps, float duration);
private:
    void updateParticles(Enemy& e, float dt, class ParticleSystem* particleSystem);
};