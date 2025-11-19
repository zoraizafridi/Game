#include "../maps/Map.hpp"
#include "../json/JSONLoader.hpp"
#include "../json/json.hpp"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <cmath>

using json = nlohmann::json;

bool Map::loadFromJSON(JSONLoader* jsonLoader, const std::string& mapId) {
    (void)jsonLoader;
    mapId_ = mapId;
    std::cout << "[Map] Loading map: " << mapId << std::endl;
    
    std::string mapFile = "data/maps.json";
    std::cout << "[Map] Looking for maps.json at: " << std::filesystem::absolute(mapFile) << std::endl;
    
    if (!std::filesystem::exists(mapFile)) {
        std::cerr << "[Map] ERROR: maps.json not found at: " << mapFile << std::endl;
        std::cerr << "[Map] Current working directory: " << std::filesystem::current_path() << std::endl;
        return loadDefault();
    }
    
    try {
        std::ifstream file(mapFile);
        if (!file.is_open()) {
            std::cerr << "[Map] ERROR: Failed to open maps.json" << std::endl;
            return loadDefault();
        }
        
        json data;
        file >> data;
        file.close();
        
        if (!data.contains("maps") || !data["maps"].is_array()) {
            std::cerr << "[Map] ERROR: Invalid format - no 'maps' array" << std::endl;
            return loadDefault();
        }
        
        bool found = false;
        for (const auto& mapData : data["maps"]) {
            if (mapData.contains("id") && mapData["id"] == mapId) {
                std::cout << "[Map] Found map: " << mapId << std::endl;
                found = true;
                if (parseMapData(mapData)) {
                    std::cout << "[Map] ✓ Successfully loaded map: " << mapId_ << std::endl;
                    return true;
                } else {
                    std::cerr << "[Map] ✗ Failed to parse map data" << std::endl;
                    return loadDefault();
                }
            }
        }
        
        if (!found) {
            std::cerr << "[Map] ERROR: Map '" << mapId << "' not found in maps.json" << std::endl;
            return loadDefault();
        }
        
    } catch (const std::exception& e) {
        std::cerr << "[Map] ERROR parsing JSON: " << e.what() << std::endl;
        return loadDefault();
    }
    
    return loadDefault();
}

bool Map::parseMapData(const json& mapData) {
    try {
        cols_ = mapData["width"];
        rows_ = mapData["height"];
        tile_ = mapData["tile_size"];
        
        std::cout << "[Map] Size: " << cols_ << "x" << rows_ << ", tile size: " << tile_ << std::endl;
        
        // Initialize grid as BLOCKED (0) - buildable for towers
        grid_.resize(cols_ * rows_, 0);
        
        // Mark PATH tiles as walkable (1) - NOT buildable
        // Calculate path bounds and mark as walkable
        if (mapData.contains("path") && mapData["path"].is_array()) {
            for (const auto& point : mapData["path"]) {
                int pathX = point["x"].get<int>();
                int pathY = point["y"].get<int>();
                
                // Mark a 3x3 area around each path point as walkable (path)
                for (int dy = -1; dy <= 1; dy++) {
                    for (int dx = -1; dx <= 1; dx++) {
                        int x = pathX + dx;
                        int y = pathY + dy;
                        if (x >= 0 && x < cols_ && y >= 0 && y < rows_) {
                            grid_[y * cols_ + x] = 1; // 1 = walkable (path)
                        }
                    }
                }
            }
        }
        
        // Additional blocked tiles from JSON (if any) - these are ALSO buildable
        if (mapData.contains("blocked_tiles") && mapData["blocked_tiles"].is_array()) {
            int blockedCount = 0;
            for (const auto& tile : mapData["blocked_tiles"]) {
                if (tile.is_array() && tile.size() >= 2) {
                    int x = tile[0];
                    int y = tile[1];
                    if (x >= 0 && x < cols_ && y >= 0 && y < rows_) {
                        grid_[y * cols_ + x] = 0; // 0 = blocked = buildable
                        blockedCount++;
                    }
                }
            }
            std::cout << "[Map] Marked " << blockedCount << " additional blocked (buildable) tiles" << std::endl;
        }
        
        // Load path waypoints
        path_.clear();
        if (mapData.contains("path") && mapData["path"].is_array()) {
            for (const auto& point : mapData["path"]) {
                float x = point["x"].get<float>() * tile_ + tile_ / 2.0f;
                float y = point["y"].get<float>() * tile_ + tile_ / 2.0f;
                path_.emplace_back(x, y);
            }
            std::cout << "[Map] ✓ Loaded path with " << path_.size() << " waypoints" << std::endl;
            
            // Print path for debugging
            for (size_t i = 0; i < path_.size(); ++i) {
                std::cout << "[Map]   Point " << i << ": (" << path_[i].x << ", " << path_[i].y << ")" << std::endl;
            }
        }
        
        // Count buildable vs path tiles
        int buildableCount = 0;
        int pathCount = 0;
        for (const auto& tile : grid_) {
            if (tile == 0) buildableCount++;
            else pathCount++;
        }
        std::cout << "[Map] Grid: " << buildableCount << " buildable tiles, " << pathCount << " path tiles" << std::endl;
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "[Map] ERROR parsing map data: " << e.what() << std::endl;
        return false;
    }
}

bool Map::loadDefault() {
    std::cout << "[Map] Loading DEFAULT map" << std::endl;
    
    cols_ = 20;
    rows_ = 15;
    tile_ = 32.0f;
    
    // Initialize as blocked (buildable)
    grid_.resize(cols_ * rows_, 0);
    
    // Simple default path
    path_.clear();
    std::vector<sf::Vector2i> pathPoints = {
        {1, 2}, {8, 2}, {8, 7}, {15, 7}, {15, 12}, {5, 12}
    };
    
    // Mark path tiles as walkable (not buildable)
    for (const auto& point : pathPoints) {
        // Mark 3x3 area around each path point
        for (int dy = -1; dy <= 1; dy++) {
            for (int dx = -1; dx <= 1; dx++) {
                int x = point.x + dx;
                int y = point.y + dy;
                if (x >= 0 && x < cols_ && y >= 0 && y < rows_) {
                    grid_[y * cols_ + x] = 1; // 1 = walkable (path)
                }
            }
        }
        
        // Add to path waypoints
        path_.emplace_back(point.x * tile_ + tile_/2, point.y * tile_ + tile_/2);
    }
    
    std::cout << "[Map] ✓ Default map: " << cols_ << "x" << rows_ << " with " << path_.size() << " waypoints" << std::endl;
    
    return true;
}

bool Map::isValidBuildPosition(const sf::Vector2f& worldPos) const {
    // Convert world position to grid coordinates
    int gridX = static_cast<int>(worldPos.x / tile_);
    int gridY = static_cast<int>(worldPos.y / tile_);
    
    // Check if within bounds
    if (gridX < 0 || gridX >= cols_ || gridY < 0 || gridY >= rows_) {
        return false;
    }
    
    // Check if tile is BLOCKED (0 = blocked = buildable for towers)
    // Towers can ONLY be placed on blocked tiles, NOT on walkable path tiles
    int index = gridY * cols_ + gridX;
    if (index >= static_cast<int>(grid_.size())) {
        return false;
    }
    
    // Return true if BLOCKED (0 = buildable), false if walkable (1 = path)
    return grid_[index] == 0;
}

sf::Vector2f Map::getSpawnPoint() const {
    if (!path_.empty()) {
        return path_[0];
    }
    return sf::Vector2f(tile_ / 2, tile_ / 2);
}

sf::Vector2f Map::getEndPoint() const {
    if (!path_.empty()) {
        return path_.back();
    }
    return sf::Vector2f((cols_ - 1) * tile_ + tile_/2, (rows_ - 1) * tile_ + tile_/2);
}