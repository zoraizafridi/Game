#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
struct SpriteComponent {
    std::string textureId;
    sf::Sprite sprite;
    int layer = 0;
    bool visible = true;
};
class RenderSystem {
public:
    RenderSystem(sf::RenderTarget& target);
    void begin();
    void submit(const SpriteComponent& sprite);
    void submitText(const sf::Text& text, int layer = 1000);
    void end();
    void render();  // ADD THIS METHOD
    void drawHealthBar(const sf::Vector2f& position, float healthPercent, float width, float height);
private:
    sf::RenderTarget& target_;
    std::vector<std::pair<int, SpriteComponent>> sprites_;
    std::vector<std::pair<int, sf::Text>> texts_;
};