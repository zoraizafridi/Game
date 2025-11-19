#include "../systems/PathfindingSystem.hpp"
#include <queue>
#include <vector>
#include <algorithm>
#include <cmath>
#include <memory>
#include <iostream>
PathfindingSystem::PathfindingSystem() : grid_(nullptr) {}
void PathfindingSystem::initialize(int cols, int rows, float tileSize) {
    grid_ = std::make_unique<Grid>(cols, rows, tileSize);
}
std::vector<Vec2> PathfindingSystem::findPath(const Vec2& start, const Vec2& goal) {
    if (!grid_) return {};
    sf::Vector2i startGrid = grid_->worldToGrid(start);
    sf::Vector2i goalGrid = grid_->worldToGrid(goal);
    // Check if start or goal are blocked
    if (!grid_->isWalkable(startGrid.x, startGrid.y) || !grid_->isWalkable(goalGrid.x, goalGrid.y)) {
        return {};
    }
    // If start and goal are the same, return direct path
    if (startGrid == goalGrid) {
        return { start, goal };
    }
    auto nodeCompare = [](const Node* a, const Node* b) { return a->f > b->f; };
    std::priority_queue<Node*, std::vector<Node*>, decltype(nodeCompare)> openList(nodeCompare);
    std::vector<std::vector<Node*>> nodes(grid_->getHeight(), std::vector<Node*>(grid_->getWidth(), nullptr));
    Node* startNode = new Node(startGrid.x, startGrid.y);
    startNode->h = heuristic(startGrid.x, startGrid.y, goalGrid.x, goalGrid.y);
    startNode->f = startNode->g + startNode->h;
    nodes[startGrid.y][startGrid.x] = startNode;
    openList.push(startNode);
    while (!openList.empty()) {
        Node* current = openList.top();
        openList.pop();
        if (current->x == goalGrid.x && current->y == goalGrid.y) {
            std::vector<Vec2> path = reconstructPath(current);
            // PHASE 3: Store for debugging
            lastPath_ = path;
            // Cleanup
            for (int y = 0; y < grid_->getHeight(); ++y) {
                for (int x = 0; x < grid_->getWidth(); ++x) {
                    delete nodes[y][x];
                }
            }   
            // PHASE 3: Apply smoothing
            lastSmoothedPath_ = smoothPath(path);
            return lastSmoothedPath_;
        }
        // Check all 8 directions
        for (int dy = -1; dy <= 1; ++dy) {
            for (int dx = -1; dx <= 1; ++dx) {
                if (dx == 0 && dy == 0) continue;
                int nx = current->x + dx;
                int ny = current->y + dy;
                if (!grid_->isWalkable(nx, ny)) continue;
                Node* neighbor = nodes[ny][nx];
                if (!neighbor) {
                    neighbor = new Node(nx, ny);
                    nodes[ny][nx] = neighbor;
                }
                // Diagonal cost is sqrt(2), straight cost is 1
                float cost = (dx != 0 && dy != 0) ? 1.414f : 1.0f;
                float tentativeG = current->g + cost;
                if (tentativeG < neighbor->g || neighbor->g == 0) {
                    neighbor->parent = current;
                    neighbor->g = tentativeG;
                    neighbor->h = heuristic(nx, ny, goalGrid.x, goalGrid.y);
                    neighbor->f = neighbor->g + neighbor->h;
                    openList.push(neighbor);
                }
            }
        }
    }
    // Cleanup if no path found
    for (int y = 0; y < grid_->getHeight(); ++y) {
        for (int x = 0; x < grid_->getWidth(); ++x) {
            delete nodes[y][x];
        }
    }   
    return {};
}
// PHASE 3: Enhanced path smoothing
std::vector<Vec2> PathfindingSystem::smoothPath(const std::vector<Vec2>& path) {
    if (path.size() < 3) return path;
    // First pass: Remove unnecessary points using line-of-sight
    std::vector<Vec2> simplified;
    simplified.push_back(path[0]);
    size_t currentIndex = 0;
    while (currentIndex < path.size() - 1) {
        size_t furthestVisible = currentIndex + 1;
        for (size_t i = currentIndex + 2; i < path.size(); ++i) {
            if (lineOfSight(path[currentIndex], path[i])) {
                furthestVisible = i;
            } else {
                break;
            }
        }   
        simplified.push_back(path[furthestVisible]);
        currentIndex = furthestVisible;
    }
    // Second pass: Apply Catmull-Rom spline for smooth curves
    if (simplified.size() >= 4) {
        return catmullRomSpline(simplified, 6);
    }   
    return simplified;
}
// PHASE 3: Catmull-Rom spline interpolation
std::vector<Vec2> PathfindingSystem::catmullRomSpline(const std::vector<Vec2>& points, int segments) {
    if (points.size() < 4) return points;
    std::vector<Vec2> smoothed;
    smoothed.push_back(points[0]);
    for (size_t i = 0; i < points.size() - 3; ++i) {
        const Vec2& p0 = points[i];
        const Vec2& p1 = points[i + 1];
        const Vec2& p2 = points[i + 2];
        const Vec2& p3 = points[i + 3];
        for (int j = 1; j <= segments; ++j) {
            float t = j / float(segments);
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
            smoothed.push_back(point);
        }
    }   
    smoothed.push_back(points.back());
    return smoothed;
}
// PHASE 3: Bezier curve interpolation (alternative method)
std::vector<Vec2> PathfindingSystem::bezierCurve(const std::vector<Vec2>& points, int segments) {
    if (points.size() < 3) return points;
    std::vector<Vec2> smoothed;
    smoothed.push_back(points[0]);
    for (size_t i = 0; i < points.size() - 2; i += 2) {
        const Vec2& p0 = points[i];
        const Vec2& p1 = points[i + 1];
        const Vec2& p2 = (i + 2 < points.size()) ? points[i + 2] : points.back();
        for (int j = 1; j <= segments; ++j) {
            float t = j / float(segments);
            float u = 1.0f - t;
            float t2 = t * t;
            float u2 = u * u;
            Vec2 point;
            point.x = u2 * p0.x + 2 * u * t * p1.x + t2 * p2.x;
            point.y = u2 * p0.y + 2 * u * t * p1.y + t2 * p2.y;
            smoothed.push_back(point);
        }
    }   
    smoothed.push_back(points.back());
    return smoothed;
}
// PHASE 3: Get interpolated position along path for smooth movement
Vec2 PathfindingSystem::getInterpolatedPosition(const std::vector<Vec2>& path, float progress) const {
    if (path.empty()) return Vec2(0, 0);
    if (path.size() == 1) return path[0];
    float totalLength = getPathLength(path);
    float targetDistance = progress * totalLength;
    float accumulated = 0.0f;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        Vec2 segment = path[i + 1] - path[i];
        float segmentLength = std::sqrt(segment.x * segment.x + segment.y * segment.y);
        if (accumulated + segmentLength >= targetDistance) {
            float segmentProgress = (targetDistance - accumulated) / segmentLength;
            return path[i] + segment * segmentProgress;
        }   
        accumulated += segmentLength;
    }   
    return path.back();
}
// PHASE 3: Calculate total path length
float PathfindingSystem::getPathLength(const std::vector<Vec2>& path) const {
    float length = 0.0f;
    for (size_t i = 0; i < path.size() - 1; ++i) {
        Vec2 segment = path[i + 1] - path[i];
        length += std::sqrt(segment.x * segment.x + segment.y * segment.y);
    }
    return length;
}
bool PathfindingSystem::lineOfSight(const Vec2& a, const Vec2& b) const {
    if (!grid_) return false;
    sf::Vector2i start = grid_->worldToGrid(a);
    sf::Vector2i end = grid_->worldToGrid(b);
    int dx = std::abs(end.x - start.x);
    int dy = std::abs(end.y - start.y);
    int sx = (start.x < end.x) ? 1 : -1;
    int sy = (start.y < end.y) ? 1 : -1;
    int err = dx - dy;
    int x = start.x;
    int y = start.y;
    while (true) {
        if (!grid_->isWalkable(x, y)) return false;
        if (x == end.x && y == end.y) break;
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x += sx;
        }
        if (e2 < dx) {
            err += dx;
            y += sy;
        }
    }
    return true;
}
float PathfindingSystem::heuristic(int x1, int y1, int x2, int y2) const {
    // Use Euclidean distance for more natural paths
    float dx = static_cast<float>(x1 - x2);
    float dy = static_cast<float>(y1 - y2);
    return std::sqrt(dx * dx + dy * dy);
}
std::vector<Vec2> PathfindingSystem::reconstructPath(Node* endNode) const {
    if (!grid_) return {};
    std::vector<Vec2> path;
    Node* current = endNode;
    while (current) {
        path.push_back(grid_->gridToWorld(current->x, current->y));
        current = current->parent;
    }
    std::reverse(path.begin(), path.end());
    return path;
}