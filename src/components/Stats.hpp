#pragma once
struct StatsComp {
    float speed;
    float damage;
    float attackRange;
    float attackSpeed;
    StatsComp(float spd = 100.0f, float dmg = 10.0f, 
              float range = 100.0f, float atkSpd = 1.0f)
        : speed(spd), damage(dmg), attackRange(range), attackSpeed(atkSpd) {}
};