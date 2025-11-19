#include "../systems/AnimationSystem.hpp"
#include <iostream>
void AnimationSystem::registerAtlas(const std::string& atlasId, const std::vector<Animation>& list) {
    auto& animMap = atlas_[atlasId];
    for (const auto& anim : list) {
        animMap[anim.name] = anim;
    }
}
void AnimationSystem::update(AnimationStateComp& state, sf::Sprite& sprite, float dt) {
    if (!state.playing) return;
    if (state.atlasId.empty()) {
        std::cerr << "AnimationSystem: No atlas set for animation state" << std::endl;
        return;
    }
    auto atlasIt = atlas_.find(state.atlasId);
    if (atlasIt == atlas_.end()) {
        std::cerr << "AnimationSystem: Atlas not found: " << state.atlasId << std::endl;
        return;
    }
    auto animIt = atlasIt->second.find(state.currentAnim);
    if (animIt == atlasIt->second.end()) {
        std::cerr << "AnimationSystem: Animation not found: " << state.currentAnim << std::endl;
        return;
    }
    const Animation& anim = animIt->second;
    state.currentTime += dt;
    if (state.currentTime >= anim.frameTime) {
        state.currentTime = 0.f;
        state.currentFrame++;
        if (state.currentFrame >= anim.frames.size()) {
            if (anim.loop) {
                state.currentFrame = 0;
            } else {
                state.playing = false;
                state.currentFrame = anim.frames.size() - 1;
            }
        }
        if (state.currentFrame < anim.frames.size()) {
            sprite.setTextureRect(anim.frames[state.currentFrame]);
        }
    }
}
void AnimationSystem::playAnimation(AnimationStateComp& state, const std::string& animationName) {
    state.currentAnim = animationName;
    state.currentFrame = 0;
    state.currentTime = 0.f;
    state.playing = true;
}
// PHASE 3: Entity-specific animation methods
void AnimationSystem::updateEnemyAnimation(AnimationStateComp& state, sf::Sprite& sprite, bool isMoving, bool isAttacking, float dt) {
    std::string targetAnim = "idle";
    if (isAttacking) {
        targetAnim = "attack";
    } else if (isMoving) {
        targetAnim = "walk";
    }
    if (targetAnim != state.currentAnim) {
        playAnimation(state, targetAnim);
    }   
    update(state, sprite, dt);
}
void AnimationSystem::updateUnitAnimation(AnimationStateComp& state, sf::Sprite& sprite, bool isMoving, bool isAttacking, float dt) {
    std::string targetAnim = "idle";
    if (isAttacking) {
        targetAnim = "attack";
    } else if (isMoving) {
        targetAnim = "walk";
    }
    if (targetAnim != state.currentAnim) {
        playAnimation(state, targetAnim);
    }
    update(state, sprite, dt);
}
void AnimationSystem::updateTowerAnimation(AnimationStateComp& state, sf::Sprite& sprite, bool isAttacking, float dt) {
    std::string targetAnim = isAttacking ? "attack" : "idle";
    if (targetAnim != state.currentAnim) {
        playAnimation(state, targetAnim);
    }
    update(state, sprite, dt);
}
std::string AnimationSystem::getAnimationForState(const std::string& entityType, bool isMoving, bool isAttacking) {
    if (isAttacking) return "attack";
    if (isMoving) return "walk";
    return "idle";
}