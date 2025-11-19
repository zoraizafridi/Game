#include "../components/SpriteComp.hpp"
void SpriteComp::setTexture(const sf::Texture& texture) {
    sprite.setTexture(texture);
}
void SpriteComp::setPosition(const sf::Vector2f& position) {
    sprite.setPosition(position);
}
void SpriteComp::setScale(const sf::Vector2f& scale) {
    sprite.setScale(scale);
}