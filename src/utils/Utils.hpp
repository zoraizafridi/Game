#pragma once
#include <SFML/System/Vector2.hpp>
#include <cmath>
using Vec2 = sf::Vector2f;
inline float dot(const Vec2& a, const Vec2& b) { return a.x*b.x + a.y*b.y; }
inline float lengthSq(const Vec2& v) { return v.x*v.x + v.y*v.y; }
inline float length(const Vec2& v) { return std::sqrt(lengthSq(v)); }
inline Vec2 normalize(const Vec2& v) {
    float len = length(v);
    return (len > 0.0001f) ? Vec2(v.x/len, v.y/len) : Vec2(0.f,0.f);
}
inline float clampf(float v, float lo, float hi) {
    return (v < lo) ? lo : (v > hi) ? hi : v;
}