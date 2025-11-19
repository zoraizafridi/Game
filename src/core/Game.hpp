#pragma once
#include <memory>
#include <vector>
#include <SFML/Graphics.hpp>

// Forward declarations
class PathfindingSystem;
class ProjectileSystem;
class EnemySystem;
class TowerSystem;
class UnitSystem;
class RenderSystem;
class AnimationSystem;
class CollisionSystem;
class ParticleSystem;
class WaveSystem;
class UIManager;
class EventBus;
class ResourceManager;
class Map;
class Enemy;
class Unit;
class Tower;
class GameStateManager;
class CameraSystem;
class InputHandler;

// Floating combat text structure
struct FloatingText {
    std::string text;
    sf::Vector2f position;
    sf::Color color;
    float lifetime;
    float maxLifetime;
    float velocityY;
    bool active;
};

class Game {
private:
    // Core Systems
    std::unique_ptr<PathfindingSystem> pathfindingSystem_;
    std::unique_ptr<ProjectileSystem> projectileSystem_;
    std::unique_ptr<EnemySystem> enemySystem_;
    std::unique_ptr<TowerSystem> towerSystem_;
    std::unique_ptr<UnitSystem> unitSystem_;
    std::unique_ptr<RenderSystem> renderSystem_;
    std::unique_ptr<AnimationSystem> animationSystem_;
    std::unique_ptr<CollisionSystem> collisionSystem_;
    std::unique_ptr<ParticleSystem> particleSystem_;
    std::unique_ptr<WaveSystem> waveSystem_;
    std::unique_ptr<UIManager> uiManager_;
    std::unique_ptr<EventBus> eventBus_;
    std::unique_ptr<ResourceManager> resourceManager_;
    std::unique_ptr<Map> map_;
    std::unique_ptr<GameStateManager> gameStateManager_;
    std::unique_ptr<CameraSystem> cameraSystem_;
    std::unique_ptr<InputHandler> inputHandler_;

    // Window and state
    sf::RenderWindow window_;
    bool running_;
    sf::Clock clock_;

    // Game state variables
    int gold_;
    int lives_;
    int currentWave_;
    float nextWaveTimer_;
    bool waveInProgress_;

    // Tower placement
    std::string selectedTowerType_;
    bool placingTower_;

    // Unit placement
    std::string selectedUnitType_;
    bool placingUnit_;

    // Tower selection & selling
    std::shared_ptr<Tower> selectedTower_;
    bool showTowerInfo_;
    std::shared_ptr<Tower> mainBaseTower_;

    // Unit selection
    std::shared_ptr<Unit> selectedUnit_;
    bool showUnitInfo_;

    // Screen effects
    sf::Vector2f cameraOffset_;
    float screenShakeTimer_;
    float screenShakeIntensity_;
    sf::View originalView_;

    // Floating combat text
    std::vector<FloatingText> floatingTexts_;

    // Animation system integration
    bool animationSystemEnabled_;

    // Debug mode
    bool debugMode_;

    // Private methods
    void createInitialTowers();
    void spawnEnemy(const std::string& enemyId);
    void updateWaveSystem(float dt);
    void updateEntities(float dt);
    void updateUI();
    void handleMouseClickWorld(const sf::Vector2f& worldPos);
    
    // Tower methods
    bool isValidTowerPosition(const sf::Vector2f& position);
    bool placeTower(const std::string& towerType, const sf::Vector2f& position);
    int getTowerCost(const std::string& towerType);
    int getTowerSellValue(const std::shared_ptr<Tower>& tower);
    std::shared_ptr<Tower> getTowerAtPosition(const sf::Vector2f& worldPos);
    void sellSelectedTower();
    void upgradeSelectedTower();
    void deselectTower();
    
    // Unit methods
    bool isValidUnitPosition(const sf::Vector2f& position);
    bool placeUnit(const std::string& unitType, const sf::Vector2f& position);
    int getUnitCost(const std::string& unitType);
    std::shared_ptr<Unit> getUnitAtPosition(const sf::Vector2f& worldPos);
    void upgradeSelectedUnit();
    void deselectUnit();
    
    // Selection methods
    void selectEntity(const sf::Vector2f& worldPos);
    void deselectAll();
    
    void startNextWave();
    void restartGame();
    void applyScreenShake(float intensity, float duration);
    void updateScreenEffects(float dt);
    void updateFloatingTexts(float dt);
    void spawnFloatingText(const std::string& text, const sf::Vector2f& position, const sf::Color& color);
    void setupEntityAnimations();
    void setupEntityTextures();
    void setupEventSubscriptions();
    void setupInputBindings();
    void setupGameCallbacks();
    void setupUICallbacks();
    void quickSave();
    void quickLoad();
    
    // Test methods
    void testPlacement();
    
    // Rendering methods
    void renderMap();
    void renderTowers();
    void renderUnits();
    void renderEnemies();
    void renderProjectiles();
    void renderParticles();
    void renderHealthBar(const sf::Vector2f& position, float healthPercent, float width, float height);
    void renderTowerPlacementPreview();
    void renderUnitPlacementPreview();
    void renderFloatingTexts();
    void renderSelectedTowerInfo();
    void renderSelectedUnitInfo();
    void renderTowerSelection();
    void renderUnitSelection();

public:
    Game();
    ~Game();
    bool initialize();
    int run();
    void render();
    sf::RenderWindow& getWindow() { return window_; }
    
    // Public methods for effects
    void triggerScreenShake(float intensity = 5.0f, float duration = 0.3f);
    void showDamageNumber(int damage, const sf::Vector2f& position);
    void showGoldText(int gold, const sf::Vector2f& position);
};