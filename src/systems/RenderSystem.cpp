#include "../systems/RenderSystem.hpp"
#include <algorithm>
RenderSystem::RenderSystem(sf::RenderTarget& target) : target_(target) {}
void RenderSystem::begin() {
    sprites_.clear();
    texts_.clear();
}
void RenderSystem::submit(const SpriteComponent& sprite) {
    if (sprite.visible) {
        sprites_.emplace_back(sprite.layer, sprite);
    }
}
void RenderSystem::submitText(const sf::Text& text, int layer) {
    texts_.emplace_back(layer, text);
}
void RenderSystem::end() {
    // Sort sprites by layer
    std::sort(sprites_.begin(), sprites_.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });
    // Sort texts by layer
    std::sort(texts_.begin(), texts_.end(),
        [](const auto& a, const auto& b) { return a.first < b.first; });
    // Draw sprites
    for (const auto& [layer, sprite] : sprites_) {
        target_.draw(sprite.sprite);
    }   
    // Draw texts
    for (const auto& [layer, text] : texts_) {
        target_.draw(text);
    }
}
// ADD THIS MISSING METHOD
void RenderSystem::render() {
    begin();
    // In a real implementation, you would submit all sprites and texts here
    // For now, this is a placeholder that just clears and prepares for rendering
    end();
}
void RenderSystem::drawHealthBar(const sf::Vector2f& position, float healthPercent, float width, float height) {
    sf::RectangleShape background(sf::Vector2f(width, height));
    background.setFillColor(sf::Color::Red);
    background.setPosition(position);
    sf::RectangleShape health(sf::Vector2f(width * healthPercent, height));
    health.setFillColor(sf::Color::Green);
    health.setPosition(position);
    target_.draw(background);
    target_.draw(health);
}