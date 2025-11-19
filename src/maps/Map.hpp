#pragma once
#include <string>
#include <vector>
#include <SFML/System/Vector2.hpp>
#include <cstdint>
#include "../json/json.hpp"

class JSONLoader;
using json = nlohmann::json;

class Map {
public:
    bool loadFromJSON(JSONLoader* jsonLoader, const std::string& mapId);
    bool loadDefault();
    const std::vector<sf::Vector2f>& getPath() const { return path_; }
    int cols() const { return cols_; }
    int rows() const { return rows_; }
    float tileSize() const { return tile_; }
    const std::vector<uint8_t>& grid() const { return grid_; }
    
    // Helper methods for validation
    bool isValidBuildPosition(const sf::Vector2f& worldPos) const;
    sf::Vector2f getSpawnPoint() const;
    sf::Vector2f getEndPoint() const;
    std::string getMapId() const { return mapId_; }
    
private:
    bool parseMapData(const json& mapData);
    std::vector<sf::Vector2f> path_;
    std::vector<uint8_t> grid_;
    int cols_ = 0;
    int rows_ = 0;
    float tile_ = 32.f;
    std::string mapId_;
};