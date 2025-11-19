#include "../utils/Spline.hpp"
#include <cmath>
std::vector<Vec2> Spline::catmullRom(const std::vector<Vec2>& pts, int samples) {
    if (pts.size() < 4) return pts;
    std::vector<Vec2> result;
    for (size_t i = 0; i < pts.size() - 3; ++i) {
        const Vec2& p0 = pts[i];
        const Vec2& p1 = pts[i + 1];
        const Vec2& p2 = pts[i + 2];
        const Vec2& p3 = pts[i + 3];        
        for (int j = 0; j <= samples; ++j) {
            float t = j / float(samples);
            float t2 = t * t;
            float t3 = t2 * t;           
            Vec2 point;
            point.x = 0.5f * ((2.0f * p1.x) +
                            (-p0.x + p2.x) * t +
                            (2.0f * p0.x - 5.0f * p1.x + 4.0f * p2.x - p3.x) * t2 +
                            (-p0.x + 3.0f * p1.x - 3.0f * p2.x + p3.x) * t3);
            point.y = 0.5f * ((2.0f * p1.y) +
                            (-p0.y + p2.y) * t +
                            (2.0f * p0.y - 5.0f * p1.y + 4.0f * p2.y - p3.y) * t2 +
                            (-p0.y + 3.0f * p1.y - 3.0f * p2.y + p3.y) * t3);
            result.push_back(point);
        }
    }
    return result;
}