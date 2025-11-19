#include "../systems/CollisionSystem.hpp"
#include <cmath>
// FIX PARAMETER TYPE
void CollisionSystem::submit(const ColliderComp& c) {
    list_.push_back(c);
}
std::vector<std::pair<int, int>> CollisionSystem::detect() {
    std::vector<std::pair<int, int>> collisions;
    for (size_t i = 0; i < list_.size(); ++i) {
        for (size_t j = i + 1; j < list_.size(); ++j) {
            const ColliderComp& a = list_[i];  // FIXED: Use ColliderComp
            const ColliderComp& b = list_[j];  // FIXED: Use ColliderComp
            // You'll need to access position differently since ColliderComp doesn't have pos
            // This needs to be coordinated with your entity system
            float dx = 0, dy = 0; // Placeholder - you need entity positions
            float distanceSq = dx * dx + dy * dy;
            float minDistance = a.radius + b.radius;       
            if (distanceSq < minDistance * minDistance) {
                collisions.emplace_back(i, j); // Use indices since no id field
            }
        }
    }   
    return collisions;
}
void CollisionSystem::clear() {
    list_.clear();
}
void CollisionSystem::update(float dt) {
    auto collisions = detect();
    // Handle collisions
    clear();
}