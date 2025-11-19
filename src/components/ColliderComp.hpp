#pragma once
struct ColliderComp {
    float radius;
    bool isTrigger;
    ColliderComp(float r = 10.0f) : radius(r), isTrigger(false) {}
};