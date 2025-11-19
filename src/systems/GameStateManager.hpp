#pragma once
#include <SFML/Graphics.hpp>
#include <functional>
#include <string>

enum class GameState {
    MENU,
    PLAYING,
    PAUSED,
    VICTORY,
    DEFEAT,
    LEVEL_SELECT
};

class GameStateManager {
public:
    GameStateManager();
    void setState(GameState newState);
    GameState getState() const { return currentState_; }
    GameState getPreviousState() const { return previousState_; }
    bool isPlaying() const;
    bool isPaused() const;
    bool isGameOver() const;
    void pause();
    void resume();
    void togglePause();
    void victory();
    void defeat();
    void restart();
    void returnToMenu();
    void setStateChangeCallback(std::function<void(GameState, GameState)> callback);
    void renderOverlay(sf::RenderWindow& window, const sf::Font* font);
    
private:
    GameState currentState_;
    GameState previousState_;
    std::function<void(GameState, GameState)> onStateChanged_;
    void renderPauseOverlay(sf::RenderWindow& window, const sf::Font* font);
    void renderVictoryOverlay(sf::RenderWindow& window, const sf::Font* font);
    void renderDefeatOverlay(sf::RenderWindow& window, const sf::Font* font);
    void renderMenuOverlay(sf::RenderWindow& window, const sf::Font* font);
};