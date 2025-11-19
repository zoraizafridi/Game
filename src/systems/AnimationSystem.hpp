#pragma once
#include <SFML/Graphics.hpp>
#include "../components/AnimationStateComp.hpp"
#include <unordered_map>
#include <string>
#include <vector>
// Remove the separate AnimationState struct since we have AnimationStateComp
struct Animation {
    std::string name;
    std::vector<sf::IntRect> frames;
    float frameTime = 0.1f;
    bool loop = true;
};
class AnimationSystem {
public:
    void registerAtlas(const std::string& atlasId, const std::vector<Animation>& list);
    void update(AnimationStateComp& state, sf::Sprite& sprite, float dt); // CHANGED PARAMETER
    void playAnimation(AnimationStateComp& state, const std::string& animationName); // CHANGED PARAMETER
    void setLoop(AnimationStateComp& state, bool loop) { /* Can add loop to component if needed */ }
    // PHASE 3: Entity animation methods
    void updateEnemyAnimation(AnimationStateComp& state, sf::Sprite& sprite, bool isMoving, bool isAttacking, float dt);
    void updateUnitAnimation(AnimationStateComp& state, sf::Sprite& sprite, bool isMoving, bool isAttacking, float dt);
    void updateTowerAnimation(AnimationStateComp& state, sf::Sprite& sprite, bool isAttacking, float dt);
private:
    std::unordered_map<std::string, std::unordered_map<std::string, Animation>> atlas_;
    // PHASE 3: Helper methods
    std::string getAnimationForState(const std::string& entityType, bool isMoving, bool isAttacking);
};