#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>
using Vec2 = sf::Vector2f;
struct Particle {
    Vec2 pos;
    Vec2 vel;
    Vec2 startPos;
    sf::Color startColor;
    sf::Color endColor;
    float life;
    float maxLife;
    float size;
    float rotation;
    float rotationSpeed;
    bool alive;  
    // PHASE 3: New particle types
    enum Type {
        SMOKE,
        FIRE,
        SPARKLE,
        BLOOD,
        SLIME,
        FROST,
        ELECTRIC
    };
    Type type;
};
class ParticleSystem {
public:
    ParticleSystem(size_t max = 1000);
    void emit(const Vec2& pos, Particle::Type type, int count = 1);
    void update(float dt);
    void clear();
    const std::vector<Particle>& getParticles() const { return parts_; }
    // PHASE 3: Specific effect methods
    void emitExplosion(const Vec2& pos, float radius);
    void emitTrail(const Vec2& pos, const Vec2& direction);
    void emitBloodSplash(const Vec2& pos);
    void emitFrostEffect(const Vec2& pos);
    void emitFireEffect(const Vec2& pos);
    void emitPoisonCloud(const Vec2& pos);
    void emitStunStars(const Vec2& pos);
private:
    std::vector<Particle> parts_;
    // PHASE 3: Helper methods
    void initParticle(Particle& p, const Vec2& pos, Particle::Type type);
};