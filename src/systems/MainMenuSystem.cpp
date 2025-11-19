#include "../systems/MainMenuSystem.hpp"
#include <iostream>
#include <cmath>

MainMenuSystem::MainMenuSystem(sf::RenderWindow& window, ResourceManager* resources)
    : window_(window), resources_(resources), currentSelection_(0) {
}

void MainMenuSystem::initialize() {
    if (!resources_) return;
    
    // Load background
    createBackground();
    createMenuItems();
    
    std::cout << "[MainMenu] Initialized with " << menuItems_.size() << " items" << std::endl;
}

void MainMenuSystem::createBackground() {
    // Create animated background
    background_.setSize(sf::Vector2f(window_.getSize().x, window_.getSize().y));
    background_.setFillColor(sf::Color(20, 30, 50));
    
    // Create particles for background effect
    for (int i = 0; i < 50; ++i) {
        BackgroundParticle particle;
        particle.position.x = std::rand() % window_.getSize().x;
        particle.position.y = std::rand() % window_.getSize().y;
        particle.velocity.x = (std::rand() % 100 - 50) / 50.0f;
        particle.velocity.y = (std::rand() % 100 - 50) / 50.0f;
        particle.size = 1.0f + (std::rand() % 30) / 10.0f;
        particle.alpha = 100 + (std::rand() % 155);
        backgroundParticles_.push_back(particle);
    }
}

void MainMenuSystem::createMenuItems() {
    if (!resources_ || !resources_->hasFont("kenney_future")) return;
    
    const sf::Font& font = resources_->getFont("kenney_future");
    float centerX = window_.getSize().x / 2.0f;
    float startY = 350.0f;
    float spacing = 70.0f;
    
    // Title
    title_.setFont(font);
    title_.setString("TOWER DEFENSE");
    title_.setCharacterSize(80);
    title_.setFillColor(sf::Color::Cyan);
    title_.setOutlineColor(sf::Color(0, 100, 150));
    title_.setOutlineThickness(3);
    sf::FloatRect titleBounds = title_.getGlobalBounds();
    title_.setPosition(centerX - titleBounds.width / 2, 100);
    
    // Subtitle
    subtitle_.setFont(font);
    subtitle_.setString("Defend Your Kingdom");
    subtitle_.setCharacterSize(24);
    subtitle_.setFillColor(sf::Color(200, 200, 200));
    sf::FloatRect subtitleBounds = subtitle_.getGlobalBounds();
    subtitle_.setPosition(centerX - subtitleBounds.width / 2, 200);
    
    // Menu items
    std::vector<std::string> itemLabels = {
        "Start New Game",
        "Load Game",
        "Settings",
        "Credits",
        "Exit"
    };
    
    for (size_t i = 0; i < itemLabels.size(); ++i) {
        MenuItem item;
        item.text.setFont(font);
        item.text.setString(itemLabels[i]);
        item.text.setCharacterSize(36);
        item.text.setFillColor(sf::Color::White);
        
        sf::FloatRect bounds = item.text.getGlobalBounds();
        item.text.setPosition(centerX - bounds.width / 2, startY + i * spacing);
        
        item.bounds = item.text.getGlobalBounds();
        menuItems_.push_back(item);
    }
    
    // Version text
    versionText_.setFont(font);
    versionText_.setString("v1.0.0 - Alpha");
    versionText_.setCharacterSize(16);
    versionText_.setFillColor(sf::Color(100, 100, 100));
    versionText_.setPosition(10, window_.getSize().y - 30);
}

void MainMenuSystem::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::KeyPressed) {
        switch (event.key.code) {
            case sf::Keyboard::Up:
                currentSelection_ = (currentSelection_ - 1 + menuItems_.size()) % menuItems_.size();
                break;
            case sf::Keyboard::Down:
                currentSelection_ = (currentSelection_ + 1) % menuItems_.size();
                break;
            case sf::Keyboard::Return:
            case sf::Keyboard::Space:
                selectCurrentItem();
                break;
            case sf::Keyboard::Escape:
                if (currentSelection_ != 4) { // If not on Exit
                    currentSelection_ = 4; // Move to Exit
                }
                break;
            default:
                break;
        }
    }
    else if (event.type == sf::Event::MouseMoved) {
        sf::Vector2f mousePos(event.mouseMove.x, event.mouseMove.y);
        for (size_t i = 0; i < menuItems_.size(); ++i) {
            if (menuItems_[i].bounds.contains(mousePos)) {
                currentSelection_ = i;
                break;
            }
        }
    }
    else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Left) {
            sf::Vector2f mousePos(event.mouseButton.x, event.mouseButton.y);
            for (size_t i = 0; i < menuItems_.size(); ++i) {
                if (menuItems_[i].bounds.contains(mousePos)) {
                    currentSelection_ = i;
                    selectCurrentItem();
                    break;
                }
            }
        }
    }
}

void MainMenuSystem::update(float dt) {
    // Update background particles
    for (auto& particle : backgroundParticles_) {
        particle.position += particle.velocity * dt * 10.0f;
        
        // Wrap around screen
        if (particle.position.x < 0) particle.position.x = window_.getSize().x;
        if (particle.position.x > window_.getSize().x) particle.position.x = 0;
        if (particle.position.y < 0) particle.position.y = window_.getSize().y;
        if (particle.position.y > window_.getSize().y) particle.position.y = 0;
    }
    
    // Animate title
    static float titlePulse = 0.0f;
    titlePulse += dt;
    float scale = 1.0f + std::sin(titlePulse * 2.0f) * 0.05f;
    title_.setScale(scale, scale);
    
    // Update menu item colors
    for (size_t i = 0; i < menuItems_.size(); ++i) {
        if (i == currentSelection_) {
            menuItems_[i].text.setFillColor(sf::Color::Yellow);
            menuItems_[i].text.setOutlineColor(sf::Color(255, 200, 0));
            menuItems_[i].text.setOutlineThickness(2);
            
            // Scale effect
            float itemScale = 1.0f + std::sin(titlePulse * 4.0f) * 0.05f;
            menuItems_[i].text.setScale(itemScale, itemScale);
        } else {
            menuItems_[i].text.setFillColor(sf::Color::White);
            menuItems_[i].text.setOutlineThickness(0);
            menuItems_[i].text.setScale(1.0f, 1.0f);
        }
    }
}

void MainMenuSystem::render() {
    // Draw background
    window_.draw(background_);
    
    // Draw background particles
    for (const auto& particle : backgroundParticles_) {
        sf::CircleShape shape(particle.size);
        shape.setPosition(particle.position);
        shape.setFillColor(sf::Color(100, 150, 255, particle.alpha));
        window_.draw(shape);
    }
    
    // Draw title
    window_.draw(title_);
    window_.draw(subtitle_);
    
    // Draw menu items
    for (auto& item : menuItems_) {
        window_.draw(item.text);
    }
    
    // Draw version
    window_.draw(versionText_);
    
    // Draw instructions
    if (resources_ && resources_->hasFont("kenney_mini")) {
        sf::Text instructions;
        instructions.setFont(resources_->getFont("kenney_mini"));
        instructions.setString("Use Arrow Keys or Mouse | Enter/Click to Select");
        instructions.setCharacterSize(14);
        instructions.setFillColor(sf::Color(150, 150, 150));
        sf::FloatRect bounds = instructions.getGlobalBounds();
        instructions.setPosition(window_.getSize().x / 2 - bounds.width / 2, window_.getSize().y - 60);
        window_.draw(instructions);
    }
}

void MainMenuSystem::selectCurrentItem() {
    switch (currentSelection_) {
        case 0: // Start New Game
            std::cout << "[MainMenu] Starting new game..." << std::endl;
            if (onStartGame_) onStartGame_();
            break;
        case 1: // Load Game
            std::cout << "[MainMenu] Loading game..." << std::endl;
            if (onLoadGame_) onLoadGame_();
            break;
        case 2: // Settings
            std::cout << "[MainMenu] Opening settings..." << std::endl;
            if (onSettings_) onSettings_();
            break;
        case 3: // Credits
            std::cout << "[MainMenu] Showing credits..." << std::endl;
            if (onCredits_) onCredits_();
            break;
        case 4: // Exit
            std::cout << "[MainMenu] Exiting..." << std::endl;
            if (onExit_) onExit_();
            break;
    }
}

void MainMenuSystem::setStartGameCallback(std::function<void()> callback) {
    onStartGame_ = callback;
}

void MainMenuSystem::setLoadGameCallback(std::function<void()> callback) {
    onLoadGame_ = callback;
}

void MainMenuSystem::setSettingsCallback(std::function<void()> callback) {
    onSettings_ = callback;
}

void MainMenuSystem::setCreditsCallback(std::function<void()> callback) {
    onCredits_ = callback;
}

void MainMenuSystem::setExitCallback(std::function<void()> callback) {
    onExit_ = callback;
}