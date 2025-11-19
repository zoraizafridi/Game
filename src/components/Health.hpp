#pragma once
struct HealthComp {
    int hp;
    int maxHp;
    HealthComp(int health = 100) : hp(health), maxHp(health) {}
    bool alive() const { return hp > 0; }
    void takeDamage(int damage) { hp -= damage; }
    void heal(int amount) { hp = std::min(hp + amount, maxHp); }
};