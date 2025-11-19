#include "../systems/StatusEffectSystem.hpp"
#include "../systems/ParticleSystem.hpp"
#include "../entities/Enemy.hpp"
#include <iostream>
void StatusEffectSystem::apply(Enemy& e, const StatusEffect& s) {
    if (!e.effects) return;
    // Check if effect already exists
    for (auto& effect : e.effects->list) {
        if (effect.type == s.type) {
            effect.value = s.value;
            effect.remaining = s.time;
            effect.duration = s.time;
            effect.effectColor = s.color;
            effect.particleInterval = s.particleEmitRate;
            return;
        }
    }   
    // Add new effect
    AppliedEffect newEffect;
    newEffect.type = s.type;
    newEffect.value = s.value;
    newEffect.remaining = s.time;
    newEffect.duration = s.time;
    newEffect.effectColor = s.color;
    newEffect.particleInterval = s.particleEmitRate;
    e.effects->list.push_back(newEffect);
}
void StatusEffectSystem::update(Enemy& e, float dt) {
    if (!e.effects) return;
    for (auto it = e.effects->list.begin(); it != e.effects->list.end(); ) {
        it->remaining -= dt;       
        if (it->remaining <= 0.f) {
            it = e.effects->list.erase(it);
        } else {
            // Apply effect
            switch (it->type) {
                case 0: // Slow
                    if (e.stats) e.stats->speed *= (1.0f - it->value);
                    break;
                case 1: // Burn
                    if (e.health) e.health->hp -= static_cast<int>(it->value * dt);
                    break;
                case 2: // Stun
                    // Skip movement updates
                    e.velocity = sf::Vector2f(0, 0);
                    break;
                case 3: // Poison
                    if (e.health) e.health->hp -= static_cast<int>(it->value * dt);
                    break;
            }
            ++it;
        }
    }
}
void StatusEffectSystem::updateVisuals(Enemy& e, float dt, ParticleSystem* particleSystem) {
    if (!e.effects || !particleSystem) return;
    updateParticles(e, dt, particleSystem);
}
void StatusEffectSystem::updateParticles(Enemy& e, float dt, ParticleSystem* particleSystem) {
    for (auto& effect : e.effects->list) {
        effect.particleTimer += dt;
        if (effect.particleTimer >= effect.particleInterval) {
            effect.particleTimer = 0.0f;           
            // Emit particles based on effect type
            switch (effect.type) {
                case 0: // Slow - frost particles
                    particleSystem->emit(e.transform->position, Particle::FROST, 1);
                    break;
                case 1: // Burn - fire particles
                    particleSystem->emit(e.transform->position, Particle::FIRE, 1);
                    break;
                case 2: // Stun - sparkle particles
                    particleSystem->emit(e.transform->position, Particle::SPARKLE, 2);
                    break;
                case 3: // Poison - slime particles
                    particleSystem->emit(e.transform->position, Particle::SLIME, 1);
                    break;
            }
        }
    }
}
// PHASE 3: Specific effect applications
void StatusEffectSystem::applySlow(Enemy& e, float strength, float duration) {
    StatusEffect effect;
    effect.type = 0;
    effect.value = strength;
    effect.time = duration;
    effect.color = sf::Color(100, 150, 255, 200); // Blue-ish
    effect.particleEmitRate = 0.2f;
    apply(e, effect);
}
void StatusEffectSystem::applyBurn(Enemy& e, float dps, float duration) {
    StatusEffect effect;
    effect.type = 1;
    effect.value = dps;
    effect.time = duration;
    effect.color = sf::Color(255, 100, 50, 200); // Orange-red
    effect.particleEmitRate = 0.15f;
    apply(e, effect);
}
void StatusEffectSystem::applyStun(Enemy& e, float duration) {
    StatusEffect effect;
    effect.type = 2;
    effect.value = 1.0f;
    effect.time = duration;
    effect.color = sf::Color(255, 255, 100, 200); // Yellow
    effect.particleEmitRate = 0.1f;
    apply(e, effect);
}
void StatusEffectSystem::applyPoison(Enemy& e, float dps, float duration) {
    StatusEffect effect;
    effect.type = 3;
    effect.value = dps;
    effect.time = duration;
    effect.color = sf::Color(50, 200, 50, 200); // Green
    effect.particleEmitRate = 0.25f;
    apply(e, effect);
}