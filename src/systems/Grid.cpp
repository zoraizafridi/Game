#include "../systems/Grid.hpp"
#include <cmath>
Grid::Grid(int width, int height, float cellSize) 
    : width_(width), height_(height), cellSize_(cellSize) {
    grid_.resize(height_, std::vector<bool>(width_, true));
}
bool Grid::isWalkable(int x, int y) const {
    if (x < 0 || x >= width_ || y < 0 || y >= height_) 
        return false;
    return grid_[y][x];
}
bool Grid::isWalkable(const sf::Vector2f& worldPos) const {
    sf::Vector2i gridPos = worldToGrid(worldPos);
    return isWalkable(gridPos.x, gridPos.y);
}
void Grid::setWalkable(int x, int y, bool walkable) {
    if (x >= 0 && x < width_ && y >= 0 && y < height_) {
        grid_[y][x] = walkable;
    }
}
void Grid::setWalkable(const sf::Vector2f& worldPos, bool walkable) {
    sf::Vector2i gridPos = worldToGrid(worldPos);
    setWalkable(gridPos.x, gridPos.y, walkable);
}
sf::Vector2f Grid::gridToWorld(int gridX, int gridY) const {
    return sf::Vector2f(
        gridX * cellSize_ + cellSize_ / 2.0f,
        gridY * cellSize_ + cellSize_ / 2.0f
    );
}
sf::Vector2i Grid::worldToGrid(const sf::Vector2f& worldPos) const {
    return sf::Vector2i(
        static_cast<int>(worldPos.x / cellSize_),
        static_cast<int>(worldPos.y / cellSize_)
    );
}