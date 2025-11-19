#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <unordered_map>
#include <vector>

class ResourceManager {
public:
    bool loadTexture(const std::string& id, const std::string& file);
    bool loadFont(const std::string& id, const std::string& file);
    bool loadAllAssets(); // NEW: Load all game assets
    const sf::Texture& getTexture(const std::string& id) const;
    const sf::Font& getFont(const std::string& id) const;
    bool hasTexture(const std::string& id) const;
    bool hasFont(const std::string& id) const;
    void clear();

private:
    std::unordered_map<std::string, sf::Texture> textures_;
    std::unordered_map<std::string, sf::Font> fonts_;
    
    bool loadEnemyTextures();    
    bool loadTowerTextures();    
    bool loadUnitTextures();     
    bool loadProjectileTextures(); 
    bool loadMapTextures();      
    bool loadUITextures();       
    bool loadAllFonts();         
};