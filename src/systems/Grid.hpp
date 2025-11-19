#pragma once
#include <vector>
#include <SFML/System/Vector2.hpp>
class Grid {
public:
    Grid(int width = 20, int height = 15, float cellSize = 32.0f);
    bool isWalkable(int x, int y) const;
    bool isWalkable(const sf::Vector2f& worldPos) const;
    void setWalkable(int x, int y, bool walkable);
    void setWalkable(const sf::Vector2f& worldPos, bool walkable);
    sf::Vector2f gridToWorld(int gridX, int gridY) const;
    sf::Vector2i worldToGrid(const sf::Vector2f& worldPos) const;
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }
    float getCellSize() const { return cellSize_; }
    const std::vector<std::vector<bool>>& getGrid() const { return grid_; }
private:
    int width_, height_;
    float cellSize_;
    std::vector<std::vector<bool>> grid_;
};