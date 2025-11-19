#pragma once
class TimeStep {
public:
    TimeStep(float dt = 0.f) : dt_(dt) {}
    float delta() const { return dt_; }
private:
    float dt_;
};