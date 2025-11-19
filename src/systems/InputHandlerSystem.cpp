#include "../systems/InputHandlerSystem.hpp"
#include <iostream>

InputHandler::InputHandler(sf::RenderWindow& window) 
    : window_(window) {
    // Initialize mouse buttons
    for (int i = 0; i < sf::Mouse::ButtonCount; ++i) {
        sf::Mouse::Button button = static_cast<sf::Mouse::Button>(i);
        mouseButtonStates_[button] = false;
        mouseButtonJustPressed_[button] = false;
        mouseButtonJustReleased_[button] = false;
    }
    
    // Initialize keys
    for (int i = 0; i < sf::Keyboard::KeyCount; ++i) {
        sf::Keyboard::Key key = static_cast<sf::Keyboard::Key>(i);
        keyStates_[key] = false;
        keyJustPressed_[key] = false;
        keyJustReleased_[key] = false;
    }
}

void InputHandler::handleEvent(const sf::Event& event) {
    if (inputBlocked_) return;
    
    switch (event.type) {
        case sf::Event::MouseMoved:
            mousePosition_ = sf::Vector2i(event.mouseMove.x, event.mouseMove.y);
            break;
            
        case sf::Event::MouseButtonPressed:
            mouseButtonStates_[event.mouseButton.button] = true;
            mouseButtonJustPressed_[event.mouseButton.button] = true;
            mousePosition_ = sf::Vector2i(event.mouseButton.x, event.mouseButton.y);
            
            if (mouseClickCallbacks_.count(event.mouseButton.button)) {
                sf::Vector2f screenPos(static_cast<float>(event.mouseButton.x), 
                                      static_cast<float>(event.mouseButton.y));
                mouseClickCallbacks_[event.mouseButton.button](screenPos);
            }
            break;
            
        case sf::Event::MouseButtonReleased:
            mouseButtonStates_[event.mouseButton.button] = false;
            mouseButtonJustReleased_[event.mouseButton.button] = true;
            break;
            
        case sf::Event::MouseWheelScrolled:
            if (event.mouseWheelScroll.wheel == sf::Mouse::VerticalWheel) {
                mouseWheelDelta_ = event.mouseWheelScroll.delta;
                if (mouseWheelCallback_) {
                    mouseWheelCallback_(mouseWheelDelta_);
                }
            }
            break;
            
        case sf::Event::KeyPressed:
            keyStates_[event.key.code] = true;
            keyJustPressed_[event.key.code] = true;
            
            if (keyPressCallbacks_.count(event.key.code)) {
                keyPressCallbacks_[event.key.code]();
            }
            break;
            
        case sf::Event::KeyReleased:
            keyStates_[event.key.code] = false;
            keyJustReleased_[event.key.code] = true;
            
            if (keyReleaseCallbacks_.count(event.key.code)) {
                keyReleaseCallbacks_[event.key.code]();
            }
            break;
            
        default:
            break;
    }
    
    // Update action states after processing events
    handleActionCallbacks();
}

void InputHandler::update() {
    clearJustPressed();
    mouseWheelDelta_ = 0.0f; // Reset wheel delta each frame
}

void InputHandler::clearJustPressed() {
    // Clear mouse just pressed/released states
    for (auto& pair : mouseButtonJustPressed_) {
        pair.second = false;
    }
    for (auto& pair : mouseButtonJustReleased_) {
        pair.second = false;
    }
    
    // Clear key just pressed/released states
    for (auto& pair : keyJustPressed_) {
        pair.second = false;
    }
    for (auto& pair : keyJustReleased_) {
        pair.second = false;
    }
    
    // Clear action just pressed/released states
    for (auto& pair : actionJustPressed_) {
        pair.second = false;
    }
    for (auto& pair : actionJustReleased_) {
        pair.second = false;
    }
}

void InputHandler::handleActionCallbacks() {
    for (const auto& binding : keyBindings_) {
        const std::string& action = binding.first;
        sf::Keyboard::Key key = binding.second;
        
        bool currentlyPressed = isKeyPressed(key);
        bool wasPressed = actionPressedState_[action];
        
        // Check for just pressed
        if (currentlyPressed && !wasPressed) {
            actionJustPressed_[action] = true;
            if (actionPressCallbacks_.count(action)) {
                actionPressCallbacks_[action]();
            }
        }
        
        // Check for just released
        if (!currentlyPressed && wasPressed) {
            actionJustReleased_[action] = true;
            if (actionReleaseCallbacks_.count(action)) {
                actionReleaseCallbacks_[action]();
            }
        }
        
        // Update current state
        actionPressedState_[action] = currentlyPressed;
    }
}

sf::Vector2f InputHandler::getMousePosition() const {
    return sf::Vector2f(static_cast<float>(mousePosition_.x), static_cast<float>(mousePosition_.y));
}

sf::Vector2f InputHandler::getWorldMousePosition() const {
    return window_.mapPixelToCoords(mousePosition_);
}

bool InputHandler::isMouseButtonPressed(sf::Mouse::Button button) const {
    auto it = mouseButtonStates_.find(button);
    return it != mouseButtonStates_.end() ? it->second : false;
}

bool InputHandler::isMouseButtonJustPressed(sf::Mouse::Button button) const {
    auto it = mouseButtonJustPressed_.find(button);
    return it != mouseButtonJustPressed_.end() ? it->second : false;
}

bool InputHandler::isMouseButtonJustReleased(sf::Mouse::Button button) const {
    auto it = mouseButtonJustReleased_.find(button);
    return it != mouseButtonJustReleased_.end() ? it->second : false;
}

bool InputHandler::isKeyPressed(sf::Keyboard::Key key) const {
    auto it = keyStates_.find(key);
    return it != keyStates_.end() ? it->second : false;
}

bool InputHandler::isKeyJustPressed(sf::Keyboard::Key key) const {
    auto it = keyJustPressed_.find(key);
    return it != keyJustPressed_.end() ? it->second : false;
}

bool InputHandler::isKeyJustReleased(sf::Keyboard::Key key) const {
    auto it = keyJustReleased_.find(key);
    return it != keyJustReleased_.end() ? it->second : false;
}

bool InputHandler::isActionPressed(const std::string& action) const {
    auto it = keyBindings_.find(action);
    if (it != keyBindings_.end()) {
        return isKeyPressed(it->second);
    }
    return false;
}

void InputHandler::bindKey(const std::string& action, sf::Keyboard::Key key) {
    keyBindings_[action] = key;
    // Initialize action state
    actionPressedState_[action] = false;
    actionJustPressed_[action] = false;
    actionJustReleased_[action] = false;
}

void InputHandler::onKeyPress(sf::Keyboard::Key key, std::function<void()> callback) {
    keyPressCallbacks_[key] = callback;
}

void InputHandler::onKeyRelease(sf::Keyboard::Key key, std::function<void()> callback) {
    keyReleaseCallbacks_[key] = callback;
}

void InputHandler::onMouseClick(sf::Mouse::Button button, std::function<void(sf::Vector2f)> callback) {
    mouseClickCallbacks_[button] = callback;
}

void InputHandler::onMouseWheel(std::function<void(float)> callback) {
    mouseWheelCallback_ = callback;
}

void InputHandler::onActionPress(const std::string& action, std::function<void()> callback) {
    actionPressCallbacks_[action] = callback;
}

void InputHandler::onActionRelease(const std::string& action, std::function<void()> callback) {
    actionReleaseCallbacks_[action] = callback;
}