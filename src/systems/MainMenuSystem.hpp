#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <vector>
#include "../core/ResourceManager.hpp"

struct MenuItem {
    sf::Text text;
    sf::FloatRect bounds;
};

struct BackgroundParticle {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float size;
    int alpha;
};

class MainMenuSystem {
public:
    MainMenuSystem(sf::RenderWindow& window, ResourceManager* resources);
    
    void initialize();
    void handleInput(const sf::Event& event);
    void update(float dt);
    void render();
    
    // Callbacks
    void setStartGameCallback(std::function<void()> callback);
    void setLoadGameCallback(std::function<void()> callback);
    void setSettingsCallback(std::function<void()> callback);
    void setCreditsCallback(std::function<void()> callback);
    void setExitCallback(std::function<void()> callback);
    
private:
    void createBackground();
    void createMenuItems();
    void selectCurrentItem();
    
    sf::RenderWindow& window_;
    ResourceManager* resources_;
    
    sf::RectangleShape background_;
    std::vector<BackgroundParticle> backgroundParticles_;
    
    sf::Text title_;
    sf::Text subtitle_;
    sf::Text versionText_;
    
    std::vector<MenuItem> menuItems_;
    size_t currentSelection_;
    
    // Callbacks
    std::function<void()> onStartGame_;
    std::function<void()> onLoadGame_;
    std::function<void()> onSettings_;
    std::function<void()> onCredits_;
    std::function<void()> onExit_;
};