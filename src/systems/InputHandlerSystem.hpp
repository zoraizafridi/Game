#pragma once
#include <SFML/Graphics.hpp>
#include <unordered_map>
#include <functional>
#include <string>

class InputHandler {
public:
    InputHandler(sf::RenderWindow& window);
    void handleEvent(const sf::Event& event);
    void update();
    
    // Mouse input
    sf::Vector2f getMousePosition() const;
    sf::Vector2f getWorldMousePosition() const;
    bool isMouseButtonPressed(sf::Mouse::Button button) const;
    bool isMouseButtonJustPressed(sf::Mouse::Button button) const;
    bool isMouseButtonJustReleased(sf::Mouse::Button button) const;
    float getMouseWheelDelta() const { return mouseWheelDelta_; }
    
    // Keyboard input
    bool isKeyPressed(sf::Keyboard::Key key) const;
    bool isKeyJustPressed(sf::Keyboard::Key key) const;
    bool isKeyJustReleased(sf::Keyboard::Key key) const;
    bool isActionPressed(const std::string& action) const;
    
    // Binding system
    void bindKey(const std::string& action, sf::Keyboard::Key key);
    
    // Callback system
    void onKeyPress(sf::Keyboard::Key key, std::function<void()> callback);
    void onKeyRelease(sf::Keyboard::Key key, std::function<void()> callback);
    void onMouseClick(sf::Mouse::Button button, std::function<void(sf::Vector2f)> callback);
    void onMouseWheel(std::function<void(float)> callback);
    void onActionPress(const std::string& action, std::function<void()> callback);
    void onActionRelease(const std::string& action, std::function<void()> callback);
    
    // Input blocking
    void setInputBlocked(bool blocked) { inputBlocked_ = blocked; }
    bool isInputBlocked() const { return inputBlocked_; }
    
private:
    void clearJustPressed();
    void handleActionCallbacks();
    
    sf::RenderWindow& window_;
    sf::Vector2i mousePosition_;
    float mouseWheelDelta_ = 0.0f;
    bool inputBlocked_ = false;
    
    // Mouse state
    std::unordered_map<sf::Mouse::Button, bool> mouseButtonStates_;
    std::unordered_map<sf::Mouse::Button, bool> mouseButtonJustPressed_;
    std::unordered_map<sf::Mouse::Button, bool> mouseButtonJustReleased_;
    
    // Keyboard state
    std::unordered_map<sf::Keyboard::Key, bool> keyStates_;
    std::unordered_map<sf::Keyboard::Key, bool> keyJustPressed_;
    std::unordered_map<sf::Keyboard::Key, bool> keyJustReleased_;
    
    // Key bindings
    std::unordered_map<std::string, sf::Keyboard::Key> keyBindings_;
    
    // Callbacks
    std::unordered_map<sf::Keyboard::Key, std::function<void()>> keyPressCallbacks_;
    std::unordered_map<sf::Keyboard::Key, std::function<void()>> keyReleaseCallbacks_;
    std::unordered_map<sf::Mouse::Button, std::function<void(sf::Vector2f)>> mouseClickCallbacks_;
    std::function<void(float)> mouseWheelCallback_;
    std::unordered_map<std::string, std::function<void()>> actionPressCallbacks_;
    std::unordered_map<std::string, std::function<void()>> actionReleaseCallbacks_;
    
    // Action state tracking
    std::unordered_map<std::string, bool> actionPressedState_;
    std::unordered_map<std::string, bool> actionJustPressed_;
    std::unordered_map<std::string, bool> actionJustReleased_;
};