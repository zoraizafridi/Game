#pragma once
#include <SFML/Graphics.hpp>
#include <string>
class SpriteComp {
public:
    std::string textureId;
    sf::Sprite sprite;
    int layer = 0;
    bool visible = true;
    SpriteComp() = default;
    void setTexture(const sf::Texture& texture);
    void setPosition(const sf::Vector2f& position);
    void setScale(const sf::Vector2f& scale);
    sf::Sprite& getSprite() { return sprite; }
};
