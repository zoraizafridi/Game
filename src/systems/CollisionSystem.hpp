#pragma once
#include "../components/ColliderComp.hpp"  // This includes the Collider struct
#include <vector>
class CollisionSystem {
private:
    std::vector<ColliderComp> list_;  // FIXED: Use ColliderComp, not Collider
public:
    void submit(const ColliderComp& c);  // FIXED: Use ColliderComp
    std::vector<std::pair<int, int>> detect();
    void clear();
    void update(float dt);
};