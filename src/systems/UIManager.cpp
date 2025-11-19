#include "UIManager.hpp"
#include "../core/ResourceManager.hpp"
#include <iostream>
#include <cmath>

void UIManager::initialize(ResourceManager* resourceManager) {
    resourceManager_ = resourceManager;
    
    if (resourceManager_->hasFont("kenney_mini")) {
        font_ = &resourceManager_->getFont("kenney_mini");
    }
    
    setupUIElements();
    setupTowerPanel();
    setupUnitPanel();
}

void UIManager::setupUIElements() {
    // Resource panel
    resourcePanel_.setSize(sf::Vector2f(200, 80));
    resourcePanel_.setPosition(10, 10);
    resourcePanel_.setFillColor(sf::Color(0, 0, 0, 150));
    resourcePanel_.setOutlineColor(sf::Color::White);
    resourcePanel_.setOutlineThickness(1);
    
    // Wave panel
    wavePanel_.setSize(sf::Vector2f(200, 60));
    wavePanel_.setPosition(220, 10);
    wavePanel_.setFillColor(sf::Color(0, 0, 0, 150));
    wavePanel_.setOutlineColor(sf::Color::Cyan);
    wavePanel_.setOutlineThickness(1);
    
    // Menu buttons
    towerMenuButton_.setSize(sf::Vector2f(120, 30));
    towerMenuButton_.setPosition(430, 10);
    towerMenuButton_.setFillColor(sf::Color(100, 100, 200, 200));
    towerMenuButton_.setOutlineColor(sf::Color::Blue);
    towerMenuButton_.setOutlineThickness(1);
    
    unitMenuButton_.setSize(sf::Vector2f(120, 30));
    unitMenuButton_.setPosition(560, 10);
    unitMenuButton_.setFillColor(sf::Color(100, 200, 100, 200));
    unitMenuButton_.setOutlineColor(sf::Color::Green);
    unitMenuButton_.setOutlineThickness(1);
    
    // Next wave button
    nextWaveButton_.setSize(sf::Vector2f(150, 40));
    nextWaveButton_.setPosition(320, 80);
    nextWaveButton_.setFillColor(sf::Color(200, 100, 50, 200));
    nextWaveButton_.setOutlineColor(sf::Color::Yellow);
    nextWaveButton_.setOutlineThickness(2);
}

void UIManager::setupTowerPanel() {
    towerSelectionPanel_.setSize(sf::Vector2f(500, 400)); // Wider for two columns
    towerSelectionPanel_.setPosition(70, 80);
    towerSelectionPanel_.setFillColor(sf::Color(0, 0, 50, 220));
    towerSelectionPanel_.setOutlineColor(sf::Color::Blue);
    towerSelectionPanel_.setOutlineThickness(3);
    
    // Define tower types and costs - TWO COLUMNS
    std::vector<std::pair<std::string, std::string>> towerTypes = {
        {"arrow_tower", "Arrow Tower"},
        {"cannon_tower", "Cannon Tower"}, 
        {"mage_tower", "Mage Tower"},
        {"ice_tower", "Ice Tower"},
        {"lightning_tower", "Lightning Tower"},
        {"poison_tower", "Poison Tower"},
        {"ballista", "Ballista"},
        {"flame_tower", "Flame Tower"},
        {"tesla_tower", "Tesla Tower"},
        {"arcane_tower", "Arcane Tower"},
        {"sniper_tower", "Sniper Tower"},
        {"artillery_tower", "Artillery Tower"}
    };
    
    // Create tower buttons in TWO COLUMNS
    for (int i = 0; i < towerTypes.size(); ++i) {
        TowerButton button;
        button.rect.setSize(sf::Vector2f(220, 30));
        
        // First column: i=0-5, Second column: i=6-11
        if (i < 6) {
            button.rect.setPosition(90, 110 + i * 35); // First column
        } else {
            button.rect.setPosition(320, 110 + (i-6) * 35); // Second column
        }
        
        button.rect.setFillColor(sf::Color(50, 50, 100, 200));
        button.rect.setOutlineColor(sf::Color::Cyan);
        button.rect.setOutlineThickness(1);
        
        button.towerType = towerTypes[i].first;
        button.name = towerTypes[i].second;
        
        // Set costs based on tower type
        if (button.towerType == "arrow_tower") button.cost = 100;
        else if (button.towerType == "cannon_tower") button.cost = 200;
        else if (button.towerType == "mage_tower") button.cost = 300;
        else if (button.towerType == "ice_tower") button.cost = 250;
        else if (button.towerType == "lightning_tower") button.cost = 400;
        else if (button.towerType == "poison_tower") button.cost = 350;
        else if (button.towerType == "ballista") button.cost = 450;
        else if (button.towerType == "flame_tower") button.cost = 500;
        else if (button.towerType == "tesla_tower") button.cost = 600;
        else if (button.towerType == "arcane_tower") button.cost = 800;
        else if (button.towerType == "sniper_tower") button.cost = 700;
        else if (button.towerType == "artillery_tower") button.cost = 1000;
        else button.cost = 100;
        
        towerButtons_.push_back(button);
    }
}

void UIManager::setupUnitPanel() {
    unitSelectionPanel_.setSize(sf::Vector2f(500, 400)); // Wider for two columns
    unitSelectionPanel_.setPosition(70, 80);
    unitSelectionPanel_.setFillColor(sf::Color(0, 50, 0, 220));
    unitSelectionPanel_.setOutlineColor(sf::Color::Green);
    unitSelectionPanel_.setOutlineThickness(3);
    
    // Define unit types and costs - TWO COLUMNS
    std::vector<std::pair<std::string, std::string>> unitTypes = {
        {"archer", "Archer"},
        {"knight", "Knight"},
        {"mage", "Mage"},
        {"rogue", "Rogue"},
        {"paladin", "Paladin"},
        {"ranger", "Ranger"},
        {"berserker", "Berserker"},
        {"priest", "Priest"},
        {"necromancer", "Necromancer"},
        {"druid", "Druid"},
        {"engineer", "Engineer"},
        {"monk", "Monk"}
    };
    
    // Create unit buttons in TWO COLUMNS
    for (int i = 0; i < unitTypes.size(); ++i) {
        UnitButton button;
        button.rect.setSize(sf::Vector2f(220, 30));
        
        // First column: i=0-5, Second column: i=6-11
        if (i < 6) {
            button.rect.setPosition(90, 110 + i * 35); // First column
        } else {
            button.rect.setPosition(320, 110 + (i-6) * 35); // Second column
        }
        
        button.rect.setFillColor(sf::Color(50, 100, 50, 200));
        button.rect.setOutlineColor(sf::Color::Yellow);
        button.rect.setOutlineThickness(1);
        
        button.unitType = unitTypes[i].first;
        button.name = unitTypes[i].second;
        
        // Set costs based on unit type
        if (button.unitType == "archer") button.cost = 80;
        else if (button.unitType == "knight") button.cost = 120;
        else if (button.unitType == "mage") button.cost = 150;
        else if (button.unitType == "rogue") button.cost = 100;
        else if (button.unitType == "paladin") button.cost = 180;
        else if (button.unitType == "ranger") button.cost = 130;
        else if (button.unitType == "berserker") button.cost = 160;
        else if (button.unitType == "priest") button.cost = 140;
        else if (button.unitType == "necromancer") button.cost = 170;
        else if (button.unitType == "druid") button.cost = 190;
        else if (button.unitType == "engineer") button.cost = 210;
        else if (button.unitType == "monk") button.cost = 220;
        else button.cost = 80;
        
        unitButtons_.push_back(button);
    }
}

void UIManager::update(float dt) {
    if (waveReady_) {
        waveReadyTimer_ += dt;
        // Make wave ready button pulse
        float alpha = 150 + 105 * sin(waveReadyTimer_ * 5);
        nextWaveButton_.setFillColor(sf::Color(200, 100, 50, static_cast<sf::Uint8>(alpha)));
    }
    
    updateButtonStates();
}

void UIManager::updateButtonStates() {
    // Update tower button colors based on affordability
    for (auto& button : towerButtons_) {
        if (gold_ >= button.cost) {
            button.rect.setFillColor(sf::Color(50, 50, 100, 200));
            button.rect.setOutlineColor(sf::Color::Cyan);
        } else {
            button.rect.setFillColor(sf::Color(100, 50, 50, 200));
            button.rect.setOutlineColor(sf::Color::Red);
        }
    }
    
    // Update unit button colors based on affordability
    for (auto& button : unitButtons_) {
        if (gold_ >= button.cost) {
            button.rect.setFillColor(sf::Color(50, 100, 50, 200));
            button.rect.setOutlineColor(sf::Color::Yellow);
        } else {
            button.rect.setFillColor(sf::Color(100, 50, 50, 200));
            button.rect.setOutlineColor(sf::Color::Red);
        }
    }
}

void UIManager::render(sf::RenderWindow& window) {
    // Always render main UI
    window.draw(resourcePanel_);
    window.draw(wavePanel_);
    window.draw(towerMenuButton_);
    window.draw(unitMenuButton_);
    
    if (waveReady_) {
        window.draw(nextWaveButton_);
    }
    
    drawMainText(window);
    
    // Render panels if visible
    if (showTowerPanel_) {
        drawTowerPanel(window);
    }
    
    if (showUnitPanel_) {
        drawUnitPanel(window);
    }
}

void UIManager::drawMainText(sf::RenderWindow& window) {
    if (!font_) return;
    
    // Gold text
    sf::Text goldText("Gold: " + std::to_string(gold_), *font_, 18);
    goldText.setPosition(20, 15);
    goldText.setFillColor(sf::Color::Yellow);
    window.draw(goldText);
    
    // Lives text
    sf::Text livesText("Lives: " + std::to_string(lives_), *font_, 18);
    livesText.setPosition(20, 45);
    livesText.setFillColor(sf::Color::Red);
    window.draw(livesText);
    
    // Wave text
    sf::Text waveText("Wave: " + std::to_string(currentWave_) + "/" + std::to_string(totalWaves_), *font_, 18);
    waveText.setPosition(230, 15);
    waveText.setFillColor(sf::Color::Cyan);
    window.draw(waveText);
    
    // Enemy count
    sf::Text enemyText("Enemies: " + std::to_string(enemiesRemaining_), *font_, 16);
    enemyText.setPosition(230, 45);
    enemyText.setFillColor(sf::Color::White);
    window.draw(enemyText);
    
    // Menu button labels
    sf::Text towerButtonText("Towers [T]", *font_, 14);
    towerButtonText.setPosition(440, 15);
    towerButtonText.setFillColor(sf::Color::White);
    window.draw(towerButtonText);
    
    sf::Text unitButtonText("Units [U]", *font_, 14);
    unitButtonText.setPosition(570, 15);
    unitButtonText.setFillColor(sf::Color::White);
    window.draw(unitButtonText);
    
    // Wave ready text
    if (waveReady_) {
        sf::Text waveReadyText("START WAVE [SPACE]", *font_, 16);
        waveReadyText.setPosition(330, 90);
        waveReadyText.setFillColor(sf::Color::White);
        window.draw(waveReadyText);
    }
}

void UIManager::drawTowerPanel(sf::RenderWindow& window) {
    window.draw(towerSelectionPanel_);
    
    if (!font_) return;
    
    // Panel title
    sf::Text title("TOWER SELECTION", *font_, 20);
    title.setPosition(200, 85);
    title.setFillColor(sf::Color::Cyan);
    window.draw(title);
    
    // Draw tower buttons
    for (const auto& button : towerButtons_) {
        window.draw(button.rect);
        
        // Button text
        sf::Text nameText(button.name + " - " + std::to_string(button.cost) + "g", *font_, 14);
        nameText.setPosition(button.rect.getPosition().x + 5, button.rect.getPosition().y + 5);
        nameText.setFillColor(sf::Color::White);
        window.draw(nameText);
    }
}

void UIManager::drawUnitPanel(sf::RenderWindow& window) {
    window.draw(unitSelectionPanel_);
    
    if (!font_) return;
    
    // Panel title
    sf::Text title("UNIT SELECTION", *font_, 20);
    title.setPosition(200, 85);
    title.setFillColor(sf::Color::Green);
    window.draw(title);
    
    // Draw unit buttons
    for (const auto& button : unitButtons_) {
        window.draw(button.rect);
        
        // Button text
        sf::Text nameText(button.name + " - " + std::to_string(button.cost) + "g", *font_, 14);
        nameText.setPosition(button.rect.getPosition().x + 5, button.rect.getPosition().y + 5);
        nameText.setFillColor(sf::Color::White);
        window.draw(nameText);
    }
}

void UIManager::handleEvent(const sf::Event& event, sf::RenderWindow& window) {
    if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
        sf::Vector2f mousePos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        handleClick(mousePos);
    }
}

// ============================================
// NEW METHODS FOR CLICK-THROUGH DETECTION
// ============================================

bool UIManager::isMouseOverUI(const sf::Vector2f& mousePos) const {
    // Check main UI buttons (always present)
    if (towerMenuButton_.getGlobalBounds().contains(mousePos)) {
        return true;
    }
    if (unitMenuButton_.getGlobalBounds().contains(mousePos)) {
        return true;
    }
    if (waveReady_ && nextWaveButton_.getGlobalBounds().contains(mousePos)) {
        return true;
    }
    
    // Check resource and wave panels
    if (resourcePanel_.getGlobalBounds().contains(mousePos)) {
        return true;
    }
    if (wavePanel_.getGlobalBounds().contains(mousePos)) {
        return true;
    }
    
    // Check tower selection panel if visible
    if (showTowerPanel_ && towerSelectionPanel_.getGlobalBounds().contains(mousePos)) {
        return true;
    }
    
    // Check unit selection panel if visible
    if (showUnitPanel_ && unitSelectionPanel_.getGlobalBounds().contains(mousePos)) {
        return true;
    }
    
    return false;
}

bool UIManager::handleClick(const sf::Vector2f& mousePos) {
    // Check tower menu button
    if (towerMenuButton_.getGlobalBounds().contains(mousePos)) {
        toggleTowerPanel();
        return true;
    }
    
    // Check unit menu button
    if (unitMenuButton_.getGlobalBounds().contains(mousePos)) {
        toggleUnitPanel();
        return true;
    }
    
    // Check next wave button
    if (waveReady_ && nextWaveButton_.getGlobalBounds().contains(mousePos)) {
        if (onStartWave_) onStartWave_();
        return true;
    }
    
    // Check tower panel buttons
    if (showTowerPanel_) {
        for (const auto& button : towerButtons_) {
            if (button.rect.getGlobalBounds().contains(mousePos)) {
                if (gold_ >= button.cost) {
                    if (onTowerSelected_) onTowerSelected_(button.towerType);
                    showTowerPanel_ = false;
                }
                return true;
            }
        }
        // Clicked inside panel but not on a button - still consume the click
        if (towerSelectionPanel_.getGlobalBounds().contains(mousePos)) {
            return true;
        }
    }
    
    // Check unit panel buttons
    if (showUnitPanel_) {
        for (const auto& button : unitButtons_) {
            if (button.rect.getGlobalBounds().contains(mousePos)) {
                if (gold_ >= button.cost) {
                    if (onUnitSelected_) onUnitSelected_(button.unitType);
                    showUnitPanel_ = false;
                }
                return true;
            }
        }
        // Clicked inside panel but not on a button - still consume the click
        if (unitSelectionPanel_.getGlobalBounds().contains(mousePos)) {
            return true;
        }
    }
    
    return false; // Click was not on UI
}

void UIManager::closeAllPanels() {
    showTowerPanel_ = false;
    showUnitPanel_ = false;
}

// ============================================
// EXISTING METHODS
// ============================================

void UIManager::setResources(int gold, int lives, int score) {
    gold_ = gold;
    lives_ = lives;
    score_ = score;
}

void UIManager::setWaveInfo(int currentWave, int totalWaves, int enemiesRemaining, float nextWaveTimer) {
    currentWave_ = currentWave;
    totalWaves_ = totalWaves;
    enemiesRemaining_ = enemiesRemaining;
    nextWaveTimer_ = nextWaveTimer;
}

void UIManager::showWaveReady(bool show) {
    waveReady_ = show;
    if (!show) {
        waveReadyTimer_ = 0.0f;
    }
}

void UIManager::setTowerSelectedCallback(std::function<void(const std::string&)> callback) {
    onTowerSelected_ = callback;
}

void UIManager::setUnitSelectedCallback(std::function<void(const std::string&)> callback) {
    onUnitSelected_ = callback;
}

void UIManager::setStartWaveCallback(std::function<void()> callback) {
    onStartWave_ = callback;
}

void UIManager::toggleTowerPanel() {
    showTowerPanel_ = !showTowerPanel_;
    if (showTowerPanel_) {
        showUnitPanel_ = false; // Close unit panel if opening tower panel
    }
    std::cout << "[UIManager] Tower panel: " << (showTowerPanel_ ? "visible" : "hidden") << std::endl;
}

void UIManager::toggleUnitPanel() {
    showUnitPanel_ = !showUnitPanel_;
    if (showUnitPanel_) {
        showTowerPanel_ = false; // Close tower panel if opening unit panel
    }
    std::cout << "[UIManager] Unit panel: " << (showUnitPanel_ ? "visible" : "hidden") << std::endl;
}