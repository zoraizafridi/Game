#pragma once
#include <string>
struct AnimationStateComp {
    bool playing;
    float currentTime;
    float frameDuration;
    int currentFrame;
    int frameCount;
    std::string currentAnim;  // ADD THIS
    std::string atlasId;      // ADD THIS
    AnimationStateComp() 
        : playing(false), currentTime(0), frameDuration(0.1f), 
          currentFrame(0), frameCount(1), currentAnim("idle"), atlasId("") {}
};