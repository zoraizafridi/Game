#pragma once
#include <SFML/System/Vector2.hpp>
#include <vector>
#include <memory>
#include "../systems/Grid.hpp"
using Vec2 = sf::Vector2f;
struct Node {
    int x, y;
    float f, g, h;
    Node* parent;
    Node(int x = 0, int y = 0) : x(x), y(y), f(0), g(0), h(0), parent(nullptr) {}
};
class PathfindingSystem {
public:
    PathfindingSystem();
    void initialize(int cols, int rows, float tileSize);
    std::vector<Vec2> findPath(const Vec2& start, const Vec2& goal);
    // PHASE 3: Enhanced path smoothing
    std::vector<Vec2> smoothPath(const std::vector<Vec2>& path);
    std::vector<Vec2> catmullRomSpline(const std::vector<Vec2>& points, int segments = 4);
    std::vector<Vec2> bezierCurve(const std::vector<Vec2>& points, int segments = 4);
    bool lineOfSight(const Vec2& a, const Vec2& b) const;
    float heuristic(int x1, int y1, int x2, int y2) const;
    std::vector<Vec2> reconstructPath(Node* endNode) const;
    // PHASE 3: Helper methods for smooth movement
    Vec2 getInterpolatedPosition(const std::vector<Vec2>& path, float progress) const;
    float getPathLength(const std::vector<Vec2>& path) const;
    // PHASE 3: Debug visualization
    std::vector<Vec2> getLastPath() const { return lastPath_; }
    std::vector<Vec2> getLastSmoothedPath() const { return lastSmoothedPath_; }
private:
    std::unique_ptr<Grid> grid_;
    // PHASE 3: Store last paths for debugging
    std::vector<Vec2> lastPath_;
    std::vector<Vec2> lastSmoothedPath_;
};