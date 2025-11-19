#include "../systems/CameraSystem.hpp"
#include <iostream>

CameraSystem::CameraSystem(sf::RenderWindow& window)
    : window_(window),
      zoomLevel_(1.0f),
      minZoom_(0.5f),
      maxZoom_(2.0f),
      zoomSpeed_(0.1f),
      panSpeed_(800.0f),
      hasBounds_(false),
      isDragging_(false) {
    defaultView_ = window_.getDefaultView();
    view_ = defaultView_;
    std::cout << "[CameraSystem] Initialized with pan speed: " << panSpeed_ << std::endl;
}

void CameraSystem::update(float dt) {
    sf::Vector2f movement(0.f, 0.f);
    
    // WASD movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        movement.y -= panSpeed_ * dt;
        std::cout << "[Camera] Moving UP" << std::endl;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        movement.y += panSpeed_ * dt;
        std::cout << "[Camera] Moving DOWN" << std::endl;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        movement.x -= panSpeed_ * dt;
        std::cout << "[Camera] Moving LEFT" << std::endl;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        movement.x += panSpeed_ * dt;
        std::cout << "[Camera] Moving RIGHT" << std::endl;
    }
    
    // Arrow keys movement
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)) {
        movement.y -= panSpeed_ * dt;
        std::cout << "[Camera] Moving UP (Arrow)" << std::endl;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)) {
        movement.y += panSpeed_ * dt;
        std::cout << "[Camera] Moving DOWN (Arrow)" << std::endl;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) {
        movement.x -= panSpeed_ * dt;
        std::cout << "[Camera] Moving LEFT (Arrow)" << std::endl;
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) {
        movement.x += panSpeed_ * dt;
        std::cout << "[Camera] Moving RIGHT (Arrow)" << std::endl;
    }
    
    if (movement.x != 0.f || movement.y != 0.f) {
        std::cout << "[Camera] Movement delta: (" << movement.x << ", " << movement.y << ")" << std::endl;
        std::cout << "[Camera] Old position: (" << view_.getCenter().x << ", " << view_.getCenter().y << ")" << std::endl;
        move(movement);
        std::cout << "[Camera] New position: (" << view_.getCenter().x << ", " << view_.getCenter().y << ")" << std::endl;
    }
}

void CameraSystem::handleInput(const sf::Event& event) {
    if (event.type == sf::Event::MouseWheelScrolled) {
        if (event.mouseWheelScroll.delta > 0) {
            zoom(1.0f - zoomSpeed_);
        } else if (event.mouseWheelScroll.delta < 0) {
            zoom(1.0f + zoomSpeed_);
        }
    }
    else if (event.type == sf::Event::MouseButtonPressed) {
        if (event.mouseButton.button == sf::Mouse::Middle) {
            isDragging_ = true;
            dragStart_ = window_.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
        }
    }
    else if (event.type == sf::Event::MouseButtonReleased) {
        if (event.mouseButton.button == sf::Mouse::Middle) {
            isDragging_ = false;
        }
    }
    else if (event.type == sf::Event::MouseMoved) {
        if (isDragging_) {
            sf::Vector2f newPos = window_.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
            sf::Vector2f delta = dragStart_ - newPos;
            move(delta);
            dragStart_ = window_.mapPixelToCoords(sf::Vector2i(event.mouseMove.x, event.mouseMove.y));
        }
    }
    else if (event.type == sf::Event::KeyPressed) {
        if (event.key.code == sf::Keyboard::Home) {
            resetView();
        }
    }
}

void CameraSystem::setPosition(const sf::Vector2f& position) {
    view_.setCenter(position);
    clampViewToBounds();
}

void CameraSystem::move(const sf::Vector2f& offset) {
    view_.move(offset);
    clampViewToBounds();
}

void CameraSystem::zoom(float factor) {
    float newZoom = zoomLevel_ * factor;
    
    if (newZoom < minZoom_) {
        factor = minZoom_ / zoomLevel_;
        newZoom = minZoom_;
    } else if (newZoom > maxZoom_) {
        factor = maxZoom_ / zoomLevel_;
        newZoom = maxZoom_;
    }
    
    view_.zoom(factor);
    zoomLevel_ = newZoom;
    
    clampViewToBounds();
}

void CameraSystem::resetZoom() {
    float resetFactor = 1.0f / zoomLevel_;
    view_.zoom(resetFactor);
    zoomLevel_ = 1.0f;
    
    clampViewToBounds();
}

void CameraSystem::centerOn(const sf::Vector2f& position) {
    setPosition(position);
}

sf::Vector2f CameraSystem::getPosition() const {
    return view_.getCenter();
}

void CameraSystem::setBounds(const sf::FloatRect& bounds) {
    bounds_ = bounds;
    hasBounds_ = true;
    clampViewToBounds();
}

void CameraSystem::setZoomLimits(float minZoom, float maxZoom) {
    minZoom_ = minZoom;
    maxZoom_ = maxZoom;
    
    if (zoomLevel_ < minZoom_) {
        float factor = minZoom_ / zoomLevel_;
        view_.zoom(factor);
        zoomLevel_ = minZoom_;
    } else if (zoomLevel_ > maxZoom_) {
        float factor = maxZoom_ / zoomLevel_;
        view_.zoom(factor);
        zoomLevel_ = maxZoom_;
    }
}

void CameraSystem::applyView() {
    window_.setView(view_);
}

void CameraSystem::resetView() {
    view_ = defaultView_;
    zoomLevel_ = 1.0f;
    window_.setView(defaultView_);
}

void CameraSystem::clampViewToBounds() {
    if (!hasBounds_) return;
    
    sf::Vector2f center = view_.getCenter();
    sf::Vector2f halfSize = view_.getSize() / 2.f;
    
    if (center.x - halfSize.x < bounds_.left) {
        center.x = bounds_.left + halfSize.x;
    }
    if (center.x + halfSize.x > bounds_.left + bounds_.width) {
        center.x = bounds_.left + bounds_.width - halfSize.x;
    }
    if (center.y - halfSize.y < bounds_.top) {
        center.y = bounds_.top + halfSize.y;
    }
    if (center.y + halfSize.y > bounds_.top + bounds_.height) {
        center.y = bounds_.top + bounds_.height - halfSize.y;
    }
    
    view_.setCenter(center);
}