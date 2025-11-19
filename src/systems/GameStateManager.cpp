#include "../systems/GameStateManager.hpp"
#include <iostream>
GameStateManager::GameStateManager() 
    : currentState_(GameState::PLAYING), previousState_(GameState::PLAYING) {}
void GameStateManager::setState(GameState newState) {
    previousState_ = currentState_;
    currentState_ = newState;
    std::cout << "[GameStateManager] State changed to: ";
    switch (currentState_) {
        case GameState::MENU: std::cout << "MENU"; break;
        case GameState::PLAYING: std::cout << "PLAYING"; break;
        case GameState::PAUSED: std::cout << "PAUSED"; break;
        case GameState::VICTORY: std::cout << "VICTORY"; break;
        case GameState::DEFEAT: std::cout << "DEFEAT"; break;
        case GameState::LEVEL_SELECT: std::cout << "LEVEL_SELECT"; break;
    }
    std::cout << std::endl;   
    if (onStateChanged_) {
        onStateChanged_(currentState_, previousState_);
    }
}
bool GameStateManager::isPlaying() const {
    return currentState_ == GameState::PLAYING;
}
bool GameStateManager::isPaused() const {
    return currentState_ == GameState::PAUSED;
}
bool GameStateManager::isGameOver() const {
    return currentState_ == GameState::VICTORY || currentState_ == GameState::DEFEAT;
}
void GameStateManager::pause() {
    if (currentState_ == GameState::PLAYING) {
        setState(GameState::PAUSED);
    }
}
void GameStateManager::resume() {
    if (currentState_ == GameState::PAUSED) {
        setState(GameState::PLAYING);
    }
}
void GameStateManager::togglePause() {
    if (isPaused()) {
        resume();
    } else if (isPlaying()) {
        pause();
    }
}
void GameStateManager::victory() {
    setState(GameState::VICTORY);
}
void GameStateManager::defeat() {
    setState(GameState::DEFEAT);
}
void GameStateManager::restart() {
    setState(GameState::PLAYING);
}
void GameStateManager::returnToMenu() {
    setState(GameState::MENU);
}
void GameStateManager::setStateChangeCallback(std::function<void(GameState, GameState)> callback) {
    onStateChanged_ = callback;
}
void GameStateManager::renderOverlay(sf::RenderWindow& window, const sf::Font* font) {
    switch (currentState_) {
        case GameState::PAUSED:
            renderPauseOverlay(window, font);
            break;
        case GameState::VICTORY:
            renderVictoryOverlay(window, font);
            break;
        case GameState::DEFEAT:
            renderDefeatOverlay(window, font);
            break;
        case GameState::MENU:
            renderMenuOverlay(window, font);
            break;
        default:
            break;
    }
}
void GameStateManager::renderPauseOverlay(sf::RenderWindow& window, const sf::Font* font) {
    // Semi-transparent overlay
    sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(sf::Color(0, 0, 0, 150));
    window.draw(overlay);
    if (!font) return;
    // Pause text
    sf::Text pauseText;
    pauseText.setFont(*font);
    pauseText.setString("PAUSED");
    pauseText.setCharacterSize(72);
    pauseText.setFillColor(sf::Color::White);
    pauseText.setPosition(
        window.getSize().x / 2 - pauseText.getGlobalBounds().width / 2,
        window.getSize().y / 2 - 100
    );
    window.draw(pauseText);   
    // Instructions
    sf::Text instructionText;
    instructionText.setFont(*font);
    instructionText.setString("Press ESC to Resume\nPress Q to Quit");
    instructionText.setCharacterSize(24);
    instructionText.setFillColor(sf::Color::White);
    instructionText.setPosition(
        window.getSize().x / 2 - instructionText.getGlobalBounds().width / 2,
        window.getSize().y / 2 + 50
    );
    window.draw(instructionText);
}
void GameStateManager::renderVictoryOverlay(sf::RenderWindow& window, const sf::Font* font) {
    sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(sf::Color(0, 100, 0, 180));
    window.draw(overlay);
    if (!font) return;
    sf::Text victoryText;
    victoryText.setFont(*font);
    victoryText.setString("VICTORY!");
    victoryText.setCharacterSize(96);
    victoryText.setFillColor(sf::Color::Yellow);
    victoryText.setPosition(
        window.getSize().x / 2 - victoryText.getGlobalBounds().width / 2,
        window.getSize().y / 2 - 100
    );
    window.draw(victoryText);   
    sf::Text instructionText;
    instructionText.setFont(*font);
    instructionText.setString("Press R to Restart\nPress Q to Quit");
    instructionText.setCharacterSize(28);
    instructionText.setFillColor(sf::Color::White);
    instructionText.setPosition(
        window.getSize().x / 2 - instructionText.getGlobalBounds().width / 2,
        window.getSize().y / 2 + 50
    );
    window.draw(instructionText);
}
void GameStateManager::renderDefeatOverlay(sf::RenderWindow& window, const sf::Font* font) {
    sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(sf::Color(100, 0, 0, 180));
    window.draw(overlay);
    if (!font) return;
    sf::Text defeatText;
    defeatText.setFont(*font);
    defeatText.setString("DEFEAT");
    defeatText.setCharacterSize(96);
    defeatText.setFillColor(sf::Color::Red);
    defeatText.setPosition(
        window.getSize().x / 2 - defeatText.getGlobalBounds().width / 2,
        window.getSize().y / 2 - 100
    );
    window.draw(defeatText);   
    sf::Text instructionText;
    instructionText.setFont(*font);
    instructionText.setString("Press R to Restart\nPress Q to Quit");
    instructionText.setCharacterSize(28);
    instructionText.setFillColor(sf::Color::White);
    instructionText.setPosition(
        window.getSize().x / 2 - instructionText.getGlobalBounds().width / 2,
        window.getSize().y / 2 + 50
    );
    window.draw(instructionText);
}
void GameStateManager::renderMenuOverlay(sf::RenderWindow& window, const sf::Font* font) {
    sf::RectangleShape overlay(sf::Vector2f(window.getSize().x, window.getSize().y));
    overlay.setFillColor(sf::Color(20, 20, 40));
    window.draw(overlay);
    if (!font) return;
    sf::Text titleText;
    titleText.setFont(*font);
    titleText.setString("TOWER DEFENSE");
    titleText.setCharacterSize(80);
    titleText.setFillColor(sf::Color::Cyan);
    titleText.setPosition(
        window.getSize().x / 2 - titleText.getGlobalBounds().width / 2,
        100
    );
    window.draw(titleText);
    sf::Text startText;
    startText.setFont(*font);
    startText.setString("Press SPACE to Start");
    startText.setCharacterSize(32);
    startText.setFillColor(sf::Color::White);
    startText.setPosition(
        window.getSize().x / 2 - startText.getGlobalBounds().width / 2,
        400
    );
    window.draw(startText);
}