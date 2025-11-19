#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include <string>

class ResourceManager;

struct TowerButton {
    sf::RectangleShape rect;
    std::string towerType;
    std::string name;
    int cost;
};

struct UnitButton {
    sf::RectangleShape rect;
    std::string unitType;
    std::string name;
    int cost;
};

class UIManager {
public:
    void initialize(ResourceManager* resourceManager);
    void update(float dt);
    void render(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event, sf::RenderWindow& window);
    
    void setResources(int gold, int lives, int score);
    void setWaveInfo(int currentWave, int totalWaves, int enemiesRemaining, float nextWaveTimer);
    void showWaveReady(bool show);
    
    // Panel controls
    void toggleTowerPanel();
    void toggleUnitPanel();
    void closeAllPanels();
    
    // Click detection - NEW
    bool isMouseOverUI(const sf::Vector2f& mousePos) const;
    bool handleClick(const sf::Vector2f& mousePos);
    
    // Callbacks
    void setTowerSelectedCallback(std::function<void(const std::string&)> callback);
    void setUnitSelectedCallback(std::function<void(const std::string&)> callback);
    void setStartWaveCallback(std::function<void()> callback);
    
private:
    void setupUIElements();
    void setupTowerPanel();
    void setupUnitPanel();
    void updateButtonStates();
    void drawMainText(sf::RenderWindow& window);
    void drawTowerPanel(sf::RenderWindow& window);
    void drawUnitPanel(sf::RenderWindow& window);
    
    ResourceManager* resourceManager_ = nullptr;
    const sf::Font* font_ = nullptr;
    
    // Main UI elements (always visible)
    sf::RectangleShape resourcePanel_;
    sf::RectangleShape wavePanel_;
    sf::RectangleShape towerMenuButton_;
    sf::RectangleShape unitMenuButton_;
    sf::RectangleShape nextWaveButton_;
    
    // Tower selection panel
    sf::RectangleShape towerSelectionPanel_;
    std::vector<TowerButton> towerButtons_;
    bool showTowerPanel_ = false;
    
    // Unit selection panel
    sf::RectangleShape unitSelectionPanel_;
    std::vector<UnitButton> unitButtons_;
    bool showUnitPanel_ = false;
    
    // Game state
    int gold_ = 0;
    int lives_ = 0;
    int score_ = 0;
    int currentWave_ = 0;
    int totalWaves_ = 0;
    int enemiesRemaining_ = 0;
    float nextWaveTimer_ = 0.0f;
    bool waveReady_ = false;
    float waveReadyTimer_ = 0.0f;
    
    // Callbacks
    std::function<void(const std::string&)> onTowerSelected_;
    std::function<void(const std::string&)> onUnitSelected_;
    std::function<void()> onStartWave_;
};