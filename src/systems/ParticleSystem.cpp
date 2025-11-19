#include "../systems/ParticleSystem.hpp"
#include "../utils/Random.hpp"
#include <cmath>
ParticleSystem::ParticleSystem(size_t max) {
    parts_.resize(max);
    for (auto& p : parts_) {
        p.alive = false;
    }
}
void ParticleSystem::emit(const Vec2& pos, Particle::Type type, int count) {
    for (int i = 0; i < count; ++i) {
        for (auto& p : parts_) {
            if (!p.alive) {
                initParticle(p, pos, type);
                break;
            }
        }
    }
}
void ParticleSystem::initParticle(Particle& p, const Vec2& pos, Particle::Type type) {
    p.pos = pos;
    p.startPos = pos;
    p.alive = true;
    p.type = type;
    p.rotation = Random::range(0.0f, 360.0f);
    p.rotationSpeed = Random::range(-180.0f, 180.0f);
    switch (type) {
        case Particle::SMOKE:
            p.vel = Vec2(Random::range(-20.f, 20.f), Random::range(-50.f, -30.f));
            p.life = p.maxLife = Random::range(1.0f, 2.0f);
            p.size = Random::range(2.0f, 8.0f);
            p.startColor = sf::Color(100, 100, 100, 200);
            p.endColor = sf::Color(50, 50, 50, 0);
            break;
        case Particle::FIRE:
            p.vel = Vec2(Random::range(-15.f, 15.f), Random::range(-40.f, -20.f));
            p.life = p.maxLife = Random::range(0.5f, 1.2f);
            p.size = Random::range(3.0f, 6.0f);
            p.startColor = sf::Color(255, 200, 50, 255);
            p.endColor = sf::Color(255, 50, 0, 0);
            break;
        case Particle::SPARKLE:
            p.vel = Vec2(Random::range(-100.f, 100.f), Random::range(-100.f, 100.f));
            p.life = p.maxLife = Random::range(0.3f, 0.8f);
            p.size = Random::range(1.0f, 3.0f);
            p.startColor = sf::Color(255, 255, 200, 255);
            p.endColor = sf::Color(255, 255, 100, 0);
            break;
        case Particle::BLOOD:
            p.vel = Vec2(Random::range(-80.f, 80.f), Random::range(-80.f, 80.f));
            p.life = p.maxLife = Random::range(0.8f, 1.5f);
            p.size = Random::range(2.0f, 5.0f);
            p.startColor = sf::Color(180, 0, 0, 255);
            p.endColor = sf::Color(100, 0, 0, 0);
            break;
        case Particle::SLIME:
            p.vel = Vec2(Random::range(-30.f, 30.f), Random::range(-30.f, 30.f));
            p.life = p.maxLife = Random::range(1.5f, 3.0f);
            p.size = Random::range(3.0f, 8.0f);
            p.startColor = sf::Color(0, 200, 50, 200);
            p.endColor = sf::Color(0, 100, 25, 0);
            break;
        case Particle::FROST:
            p.vel = Vec2(Random::range(-25.f, 25.f), Random::range(-25.f, 25.f));
            p.life = p.maxLife = Random::range(1.0f, 2.0f);
            p.size = Random::range(2.0f, 6.0f);
            p.startColor = sf::Color(200, 230, 255, 200);
            p.endColor = sf::Color(100, 150, 255, 0);
            break;           
        case Particle::ELECTRIC:
            p.vel = Vec2(Random::range(-150.f, 150.f), Random::range(-150.f, 150.f));
            p.life = p.maxLife = Random::range(0.2f, 0.5f);
            p.size = Random::range(1.0f, 4.0f);
            p.startColor = sf::Color(200, 230, 255, 255);
            p.endColor = sf::Color(100, 150, 255, 0);
            break;
    }
}
void ParticleSystem::update(float dt) {
    for (auto& p : parts_) {
        if (p.alive) {
            // Apply velocity
            p.pos += p.vel * dt;
            // Apply rotation
            p.rotation += p.rotationSpeed * dt;
            // Apply gravity for some types
            if (p.type == Particle::BLOOD || p.type == Particle::SLIME) {
                p.vel.y += 98.0f * dt; // Gravity
            }           
            // Update life
            p.life -= dt;
            if (p.life <= 0.f) {
                p.alive = false;
            }
        }
    }
}
// PHASE 3: Specific effect methods
void ParticleSystem::emitExplosion(const Vec2& pos, float radius) {
    emit(pos, Particle::FIRE, 20);
    emit(pos, Particle::SMOKE, 10);
    emit(pos, Particle::SPARKLE, 15);
}
void ParticleSystem::emitTrail(const Vec2& pos, const Vec2& direction) {
    emit(pos, Particle::SMOKE, 2);
}
void ParticleSystem::emitBloodSplash(const Vec2& pos) {
    emit(pos, Particle::BLOOD, 15);
}
void ParticleSystem::emitFrostEffect(const Vec2& pos) {
    emit(pos, Particle::FROST, 12);
}
void ParticleSystem::emitFireEffect(const Vec2& pos) {
    emit(pos, Particle::FIRE, 8);
}
void ParticleSystem::emitPoisonCloud(const Vec2& pos) {
    emit(pos, Particle::SLIME, 10);
}
void ParticleSystem::emitStunStars(const Vec2& pos) {
    emit(pos, Particle::SPARKLE, 8);
}
void ParticleSystem::clear() {
    for (auto& p : parts_) {
        p.alive = false;
    }
}