#pragma once
#include <SFML/Graphics.hpp>

class CameraSystem {
public:
    CameraSystem(sf::RenderWindow& window);
    
    void update(float dt);
    void handleInput(const sf::Event& event);
    
    // Camera control
    void setPosition(const sf::Vector2f& position);
    void move(const sf::Vector2f& offset);
    void zoom(float factor);
    void resetZoom();
    void centerOn(const sf::Vector2f& position);
    
    // Getters
    sf::Vector2f getPosition() const;
    float getZoomLevel() const { return zoomLevel_; }
    const sf::View& getView() const { return view_; }
    
    // Configuration
    void setBounds(const sf::FloatRect& bounds);
    void setZoomLimits(float minZoom, float maxZoom);
    void setPanSpeed(float speed) { panSpeed_ = speed; }
    void setZoomSpeed(float speed) { zoomSpeed_ = speed; }
    
    // Apply view to window
    void applyView();
    void resetView();
    
    // Mouse position helpers
    sf::Vector2f screenToWorld(const sf::Vector2i& screenPos) const;
    sf::Vector2i worldToScreen(const sf::Vector2f& worldPos) const;
    
private:
    void clampViewToBounds();
    
    sf::RenderWindow& window_;
    sf::View view_;
    sf::View defaultView_;
    
    float zoomLevel_;
    float minZoom_;
    float maxZoom_;
    float zoomSpeed_;
    float panSpeed_;
    
    sf::FloatRect bounds_;
    bool hasBounds_;
    
    // Mouse drag support
    sf::Vector2f dragStart_;
    bool isDragging_;
};