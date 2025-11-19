#include "../core/Game.hpp"
#include "../systems/PathfindingSystem.hpp"
#include "../systems/ProjectileSystem.hpp"
#include "../systems/EnemySystem.hpp"
#include "../systems/TowerSystem.hpp"
#include "../systems/UnitSystem.hpp"
#include "../systems/RenderSystem.hpp"
#include "../systems/AnimationSystem.hpp"
#include "../systems/CollisionSystem.hpp"
#include "../systems/ParticleSystem.hpp"
#include "../systems/WaveSystem.hpp"
#include "../systems/UIManager.hpp"
#include "../systems/AnimationAtlasLoader.hpp"
#include "../systems/UpgradeSystem.hpp"
#include "../systems/GameStateManager.hpp"
#include "../systems/SaveLoadSystem.hpp"
#include "../systems/CameraSystem.hpp"
#include "../systems/InputHandlerSystem.hpp"
#include "../core/EventBus.hpp"
#include "../core/ResourceManager.hpp"
#include "../core/GameEvents.hpp"
#include "../entities/Enemy.hpp"
#include "../entities/Unit.hpp" 
#include "../entities/Tower.hpp"
#include "../components/Transform.hpp"
#include "../components/Health.hpp"
#include "../components/Stats.hpp"
#include "../maps/Map.hpp"
#include "../json/JSONLoader.hpp"
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

Game::Game() : running_(true), gold_(500), lives_(20), currentWave_(0), 
               nextWaveTimer_(0.0f), waveInProgress_(false), 
               placingTower_(false), placingUnit_(false),
               screenShakeTimer_(0.0f), screenShakeIntensity_(0.0f),
               animationSystemEnabled_(false), debugMode_(true),
               selectedTower_(nullptr), showTowerInfo_(false), 
               selectedUnit_(nullptr), showUnitInfo_(false),
               mainBaseTower_(nullptr) {
    std::cout << "[Game] Constructor - Creating window..." << std::endl;
    window_.create(sf::VideoMode(640, 480), "Tower Defense");
    window_.setFramerateLimit(60);
    std::srand(static_cast<unsigned int>(std::time(nullptr)));
}

Game::~Game() {
    std::cout << "[Game] Destructor called" << std::endl;
}

bool Game::initialize() {
    std::cout << "[Game] ========================================" << std::endl;
    std::cout << "[Game] STARTING INITIALIZATION" << std::endl;
    std::cout << "[Game] ========================================" << std::endl;
    
    try {
        // Phase 1: Create core systems
        std::cout << "\n[Game] PHASE 1: Creating Core Systems..." << std::endl;
        
        pathfindingSystem_ = std::make_unique<PathfindingSystem>();
        projectileSystem_ = std::make_unique<ProjectileSystem>();
        enemySystem_ = std::make_unique<EnemySystem>();
        towerSystem_ = std::make_unique<TowerSystem>();
        unitSystem_ = std::make_unique<UnitSystem>();
        renderSystem_ = std::make_unique<RenderSystem>(window_);
        animationSystem_ = std::make_unique<AnimationSystem>();
        collisionSystem_ = std::make_unique<CollisionSystem>();
        particleSystem_ = std::make_unique<ParticleSystem>();
        waveSystem_ = std::make_unique<WaveSystem>();
        uiManager_ = std::make_unique<UIManager>();
        eventBus_ = std::make_unique<EventBus>();
        resourceManager_ = std::make_unique<ResourceManager>();
        map_ = std::make_unique<Map>();
        gameStateManager_ = std::make_unique<GameStateManager>();
        cameraSystem_ = std::make_unique<CameraSystem>(window_);
        inputHandler_ = std::make_unique<InputHandler>(window_);
        enemySystem_->setEventBus(eventBus_.get()); 
        std::cout << "[Game] ✓✓✓ All Core Systems Created!" << std::endl;

        // Phase 2: Load assets
        std::cout << "\n[Game] PHASE 2: Loading Assets..." << std::endl;
        if (!resourceManager_->loadAllAssets()) {
            std::cerr << "[Game] ⚠ WARNING: Some assets failed to load" << std::endl;
        }
        
        // Phase 3: Load game data
        std::cout << "\n[Game] PHASE 3: Loading Game Data..." << std::endl;
        JSONLoader jsonLoader;
        if (!jsonLoader.loadAllGameData()) {
            std::cerr << "[Game] ⚠ WARNING: Some game data failed to load" << std::endl;
        }

        // Phase 4: Initialize systems
        std::cout << "\n[Game] PHASE 4: Initializing Systems..." << std::endl;
        
        uiManager_->initialize(resourceManager_.get());
        enemySystem_->initialize(projectileSystem_.get(), unitSystem_.get());
        towerSystem_->initialize(enemySystem_.get(), projectileSystem_.get());
        unitSystem_->initialize(enemySystem_.get(), projectileSystem_.get());
        projectileSystem_->initialize(enemySystem_.get(), particleSystem_.get());
        pathfindingSystem_->initialize(20, 15, 32.0f);
        waveSystem_->load(jsonLoader.getAllWaves());
        AnimationAtlasLoader::loadAllAtlases(animationSystem_.get());
        
        waveSystem_->setSpawnCallback([this](const std::string& enemyId) {
            this->spawnEnemy(enemyId);
        });
        
        // Load map
        if (!map_->loadFromJSON(&jsonLoader, "forest_path")) {
            std::cerr << "[Game] Map load failed, using default" << std::endl;
            map_->loadDefault();
        }
        
        // Setup camera with correct map bounds
        float mapWidth = map_->cols() * map_->tileSize();
        float mapHeight = map_->rows() * map_->tileSize();
        
        std::cout << "[Game] Map size: " << mapWidth << "x" << mapHeight << std::endl;
        std::cout << "[Game] Map grid: " << map_->cols() << "x" << map_->rows() << " tiles" << std::endl;
        
        cameraSystem_->setZoomLimits(1.0f, 2.0f);
        cameraSystem_->setPanSpeed(500.0f);
        cameraSystem_->centerOn(sf::Vector2f(mapWidth / 2, mapHeight / 2));
        
        std::cout << "[Game] Camera centered at: (" << mapWidth/2 << ", " << mapHeight/2 << ")" << std::endl;

        // Phase 5: Set up events
        std::cout << "\n[Game] PHASE 5: Setting Up Events..." << std::endl;
        setupEventSubscriptions();
        setupInputBindings();
        setupGameCallbacks();
        setupUICallbacks();

        // Phase 6: Create initial game state
        std::cout << "\n[Game] PHASE 6: Creating Initial State..." << std::endl;
        createInitialTowers();
        
        originalView_ = window_.getDefaultView();
        setupEntityAnimations();
        setupEntityTextures();
        
        currentWave_ = 0;
        nextWaveTimer_ = 0.0f;
        waveInProgress_ = false;

        std::cout << "\n[Game] ========================================" << std::endl;
        std::cout << "[Game] ✓✓✓ INITIALIZATION COMPLETE ✓✓✓" << std::endl;
        std::cout << "[Game] ========================================" << std::endl;
        std::cout << "\n[Game] CONTROLS:" << std::endl;
        std::cout << "  WASD/Arrows - Move Camera" << std::endl;
        std::cout << "  Mouse Wheel - Zoom Camera" << std::endl;
        std::cout << "  1-9,0,-,= - Quick Tower Select (All 12 Types)" << std::endl;
        std::cout << "  F1-F12 - Quick Unit Select (All 12 Types)" << std::endl;
        std::cout << "  T - Open Tower Menu (All 12 Types)" << std::endl;
        std::cout << "  U - Open Unit Menu (All 12 Types)" << std::endl;
        std::cout << "  Left Click - Place Tower/Unit OR Select" << std::endl;
        std::cout << "  Right Click - Select Tower/Unit" << std::endl;
        std::cout << "  X or DELETE - Sell Selected Tower" << std::endl;
        std::cout << "  E - Upgrade Selected Tower/Unit" << std::endl;
        std::cout << "  Space - Start Wave" << std::endl;
        std::cout << "  ESC - Cancel/Pause/Resume" << std::endl;
        std::cout << "  \\ - Toggle Debug Mode" << std::endl;
        std::cout << "  F5 - Quick Save" << std::endl;
        std::cout << "  F9 - Quick Load" << std::endl;
        std::cout << "  R - Restart (any time)" << std::endl;
        std::cout << "  Q - Quit (any time)" << std::endl;
        std::cout << "[Game] ========================================\n" << std::endl;
        
        // TEMPORARY: Test placement cursor
        testPlacement();
        
        return true;
        
    } catch (const std::exception& e) {
        std::cerr << "\n[Game] ✗✗✗ EXCEPTION: " << e.what() << std::endl;
        return false;
    }
}

void Game::setupUICallbacks() {
    std::cout << "[Game] Setting up UI callbacks..." << std::endl;
    
    // Tower selection from UI
    uiManager_->setTowerSelectedCallback([this](const std::string& towerType) {
        placingTower_ = true;
        placingUnit_ = false;
        selectedTowerType_ = towerType;
        selectedUnitType_.clear();
        deselectAll();
        std::cout << "[Game] Tower type selected from UI: " << towerType << std::endl;
    });
    
    // Unit selection from UI
    uiManager_->setUnitSelectedCallback([this](const std::string& unitType) {
        placingUnit_ = true;
        placingTower_ = false;
        selectedUnitType_ = unitType;
        selectedTowerType_.clear();
        deselectAll();
        std::cout << "[Game] Unit type selected from UI: " << unitType << std::endl;
    });
    
    // Start wave from UI
    uiManager_->setStartWaveCallback([this]() {
        if (!waveInProgress_ && nextWaveTimer_ <= 0.0f && currentWave_ < waveSystem_->getTotalWaves()) {
            std::cout << "[Game] Starting wave from UI: " << (currentWave_ + 1) << std::endl;
            startNextWave();
        }
    });
}

void Game::testPlacement() {
    std::cout << "[TEST] Testing placement mode..." << std::endl;
    placingTower_ = true;
    placingUnit_ = false;
    selectedTowerType_ = "arrow_tower";
    std::cout << "[TEST] Placement mode activated - should see green circle cursor" << std::endl;
}

// Replace your setupInputBindings() in Game.cpp with this corrected version:

void Game::setupInputBindings() {
    std::cout << "[Game] Setting up input bindings..." << std::endl;
    
    // Mouse click - passes screen coordinates, we convert to world
    inputHandler_->onMouseClick(sf::Mouse::Left, [this](sf::Vector2f screenPos) {
        // First check if click is on UI
        if (uiManager_->isMouseOverUI(screenPos)) {
            // Let UI handle it and don't process world click
            uiManager_->handleClick(screenPos);
            return;
        }
        
        // If click is outside UI, handle world click
        sf::Vector2i pixelPos(static_cast<int>(screenPos.x), static_cast<int>(screenPos.y));
        sf::Vector2f worldPos = window_.mapPixelToCoords(pixelPos);
        handleMouseClickWorld(worldPos);
    });
    
    // Tower/Unit selection with right-click
    inputHandler_->onMouseClick(sf::Mouse::Right, [this](sf::Vector2f screenPos) {
        sf::Vector2i pixelPos(static_cast<int>(screenPos.x), static_cast<int>(screenPos.y));
        sf::Vector2f worldPos = window_.mapPixelToCoords(pixelPos);
        selectEntity(worldPos);
    });
    
    // Mouse wheel for camera zoom
    inputHandler_->onMouseWheel([this](float delta) {
        if (gameStateManager_->isPlaying() || gameStateManager_->isPaused()) {
            cameraSystem_->zoom(delta > 0 ? 1.1f : 0.9f);
        }
    });

    // ========== ALL 12 TOWERS ==========
    // Quick tower selection (1-9, 0, -, =)
    inputHandler_->onKeyPress(sf::Keyboard::Num1, [this]() {
        if (gameStateManager_->isPlaying()) {
            // Clear any existing selections FIRST
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            // Now set placement mode
            placingTower_ = true;
            placingUnit_ = false;
            selectedTowerType_ = "arrow_tower";
            selectedUnitType_.clear();
            
            std::cout << "[Input] Arrow tower selected (100g)" << std::endl;
            std::cout << "[DEBUG] placingTower_=" << placingTower_ << ", placingUnit_=" << placingUnit_ << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::Num2, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingTower_ = true;
            placingUnit_ = false;
            selectedTowerType_ = "cannon_tower";
            selectedUnitType_.clear();
            std::cout << "[Input] Cannon tower selected (200g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::Num3, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingTower_ = true;
            placingUnit_ = false;
            selectedTowerType_ = "mage_tower";
            selectedUnitType_.clear();
            std::cout << "[Input] Mage tower selected (300g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::Num4, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingTower_ = true;
            placingUnit_ = false;
            selectedTowerType_ = "ice_tower";
            selectedUnitType_.clear();
            std::cout << "[Input] Ice tower selected (250g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::Num5, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingTower_ = true;
            placingUnit_ = false;
            selectedTowerType_ = "lightning_tower";
            selectedUnitType_.clear();
            std::cout << "[Input] Lightning tower selected (400g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::Num6, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingTower_ = true;
            placingUnit_ = false;
            selectedTowerType_ = "poison_tower";
            selectedUnitType_.clear();
            std::cout << "[Input] Poison tower selected (350g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::Num7, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingTower_ = true;
            placingUnit_ = false;
            selectedTowerType_ = "ballista";
            selectedUnitType_.clear();
            std::cout << "[Input] Ballista selected (450g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::Num8, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingTower_ = true;
            placingUnit_ = false;
            selectedTowerType_ = "flame_tower";
            selectedUnitType_.clear();
            std::cout << "[Input] Flame tower selected (500g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::Num9, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingTower_ = true;
            placingUnit_ = false;
            selectedTowerType_ = "tesla_tower";
            selectedUnitType_.clear();
            std::cout << "[Input] Tesla tower selected (600g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::Num0, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingTower_ = true;
            placingUnit_ = false;
            selectedTowerType_ = "arcane_tower";
            selectedUnitType_.clear();
            std::cout << "[Input] Arcane tower selected (800g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::Dash, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingTower_ = true;
            placingUnit_ = false;
            selectedTowerType_ = "sniper_tower";
            selectedUnitType_.clear();
            std::cout << "[Input] Sniper tower selected (700g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::Equal, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingTower_ = true;
            placingUnit_ = false;
            selectedTowerType_ = "artillery_tower";
            selectedUnitType_.clear();
            std::cout << "[Input] Artillery tower selected (1000g)" << std::endl;
        }
    });

    // ========== ALL 12 UNITS ==========
    // Quick unit selection (F1-F12)
    inputHandler_->onKeyPress(sf::Keyboard::F1, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingUnit_ = true;
            placingTower_ = false;
            selectedUnitType_ = "archer";
            selectedTowerType_.clear();
            std::cout << "[Input] Archer selected (80g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::F2, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingUnit_ = true;
            placingTower_ = false;
            selectedUnitType_ = "knight";
            selectedTowerType_.clear();
            std::cout << "[Input] Knight selected (120g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::F3, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingUnit_ = true;
            placingTower_ = false;
            selectedUnitType_ = "mage";
            selectedTowerType_.clear();
            std::cout << "[Input] Mage unit selected (150g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::F4, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingUnit_ = true;
            placingTower_ = false;
            selectedUnitType_ = "rogue";
            selectedTowerType_.clear();
            std::cout << "[Input] Rogue selected (100g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::F5, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingUnit_ = true;
            placingTower_ = false;
            selectedUnitType_ = "paladin";
            selectedTowerType_.clear();
            std::cout << "[Input] Paladin selected (180g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::F6, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingUnit_ = true;
            placingTower_ = false;
            selectedUnitType_ = "ranger";
            selectedTowerType_.clear();
            std::cout << "[Input] Ranger selected (130g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::F7, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingUnit_ = true;
            placingTower_ = false;
            selectedUnitType_ = "berserker";
            selectedTowerType_.clear();
            std::cout << "[Input] Berserker selected (160g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::F8, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingUnit_ = true;
            placingTower_ = false;
            selectedUnitType_ = "priest";
            selectedTowerType_.clear();
            std::cout << "[Input] Priest selected (140g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::F9, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingUnit_ = true;
            placingTower_ = false;
            selectedUnitType_ = "necromancer";
            selectedTowerType_.clear();
            std::cout << "[Input] Necromancer selected (170g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::F10, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingUnit_ = true;
            placingTower_ = false;
            selectedUnitType_ = "druid";
            selectedTowerType_.clear();
            std::cout << "[Input] Druid selected (190g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::F11, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingUnit_ = true;
            placingTower_ = false;
            selectedUnitType_ = "engineer";
            selectedTowerType_.clear();
            std::cout << "[Input] Engineer selected (210g)" << std::endl;
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::F12, [this]() {
        if (gameStateManager_->isPlaying()) {
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
            
            placingUnit_ = true;
            placingTower_ = false;
            selectedUnitType_ = "monk";
            selectedTowerType_.clear();
            std::cout << "[Input] Monk selected (220g)" << std::endl;
        }
    });
    
    // Tower menu (T key)
    inputHandler_->onKeyPress(sf::Keyboard::T, [this]() {
        if (gameStateManager_->isPlaying() || gameStateManager_->isPaused()) {
            uiManager_->toggleTowerPanel();
            std::cout << "[Input] Tower menu toggled" << std::endl;
        }
    });
    
    // Unit menu (U key)
    inputHandler_->onKeyPress(sf::Keyboard::U, [this]() {
        if (gameStateManager_->isPlaying() || gameStateManager_->isPaused()) {
            uiManager_->toggleUnitPanel();
            std::cout << "[Input] Unit menu toggled" << std::endl;
        }
    });
    
    // Upgrade (E key)
    inputHandler_->onKeyPress(sf::Keyboard::E, [this]() {
        if (gameStateManager_->isPlaying()) {
            if (selectedTower_) {
                upgradeSelectedTower();
            } else if (selectedUnit_) {
                upgradeSelectedUnit();
            }
        }
    });
    
    // Wave control
    inputHandler_->onKeyPress(sf::Keyboard::Space, [this]() {
        if (gameStateManager_->isPlaying() && !waveInProgress_ && nextWaveTimer_ <= 0.0f && currentWave_ < waveSystem_->getTotalWaves()) {
            std::cout << "[Input] Starting wave " << (currentWave_ + 1) << std::endl;
            startNextWave();
        }
    });
    
    // Pause/Cancel
    inputHandler_->onKeyPress(sf::Keyboard::Escape, [this]() {
        if (gameStateManager_->isPlaying()) {
            if (selectedTower_ || selectedUnit_) {
                deselectAll();
                std::cout << "[Input] Deselected all" << std::endl;
            } else if (placingTower_ || placingUnit_) {
                placingTower_ = false;
                placingUnit_ = false;
                selectedTowerType_.clear();
                selectedUnitType_.clear();
                std::cout << "[Input] Cancelled placement" << std::endl;
            } else {
                gameStateManager_->togglePause();
                std::cout << "[Input] Game " << (gameStateManager_->isPaused() ? "paused" : "resumed") << std::endl;
            }
        } else if (gameStateManager_->isPaused()) {
            gameStateManager_->togglePause();
            std::cout << "[Input] Game resumed" << std::endl;
        }
    });
    
    // Sell tower (DELETE or X)
    inputHandler_->onKeyPress(sf::Keyboard::Delete, [this]() {
        if (gameStateManager_->isPlaying() && selectedTower_) {
            sellSelectedTower();
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::X, [this]() {
        if (gameStateManager_->isPlaying() && selectedTower_) {
            sellSelectedTower();
        }
    });
    
    // Restart (R key)
    inputHandler_->onKeyPress(sf::Keyboard::R, [this]() {
        std::cout << "[Input] Restarting game..." << std::endl;
        restartGame();
    });
    
    // Quit (Q key)
    inputHandler_->onKeyPress(sf::Keyboard::Q, [this]() {
        std::cout << "[Input] Quitting game..." << std::endl;
        window_.close();
    });
    
    // Debug toggle (Backslash key)
    inputHandler_->onKeyPress(sf::Keyboard::Backslash, [this]() {
        debugMode_ = !debugMode_;
        std::cout << "[Input] Debug mode: " << (debugMode_ ? "ON" : "OFF") << std::endl;
    });
    
    // Save/Load - NOTE: F5 and F9 might conflict with unit selection!
    // Consider changing these to different keys
    inputHandler_->onKeyPress(sf::Keyboard::F5, [this]() {
        if (gameStateManager_->isPlaying() || gameStateManager_->isPaused()) {
            // Only save if not in placement mode
            if (!placingUnit_ && !placingTower_) {
                quickSave();
            }
        }
    });
    
    inputHandler_->onKeyPress(sf::Keyboard::F9, [this]() {
        if (gameStateManager_->isPlaying() || gameStateManager_->isPaused()) {
            // Only load if not in placement mode
            if (!placingUnit_ && !placingTower_) {
                quickLoad();
            }
        }
    });
}

void Game::render() {
    window_.clear(sf::Color::Black);
    
    // ==== WORLD RENDERING ====
    cameraSystem_->applyView();
    
    // Apply screen shake if active
    if (screenShakeTimer_ > 0.0f) {
        sf::View shakeView = window_.getView();
        shakeView.move(cameraOffset_);
        window_.setView(shakeView);
    }
    
    // Render game world
    renderMap();
    renderTowers();
    renderUnits();
    renderEnemies();
    renderProjectiles();
    renderParticles();
    
    // CRITICAL: Make sure these are being called
    if (placingTower_) {
        std::cout << "[DEBUG] Rendering tower placement preview" << std::endl;
        renderTowerPlacementPreview();
    }
    
    if (placingUnit_) {
        std::cout << "[DEBUG] Rendering unit placement preview" << std::endl;
        renderUnitPlacementPreview();
    }
    
    // Render selection highlights in world space
    if (selectedTower_ && showTowerInfo_) {
        renderTowerSelection();
    }
    
    if (selectedUnit_ && showUnitInfo_) {
        renderUnitSelection();
    }
    
    // ==== UI RENDERING ====
    window_.setView(originalView_);
    
    // Show current placement mode in UI
    if (placingTower_ || placingUnit_) {
        if (resourceManager_->hasFont("kenney_mini")) {
            const sf::Font& font = resourceManager_->getFont("kenney_mini");
            sf::Text modeText;
            modeText.setFont(font);
            
            if (placingTower_) {
                modeText.setString("Placing: " + selectedTowerType_ + " (" + std::to_string(getTowerCost(selectedTowerType_)) + "g)");
                modeText.setFillColor(sf::Color::Yellow);
            } else {
                modeText.setString("Placing: " + selectedUnitType_ + " (" + std::to_string(getUnitCost(selectedUnitType_)) + "g)");
                modeText.setFillColor(sf::Color::Cyan);
            }
            
            modeText.setCharacterSize(18);
            modeText.setPosition(10, window_.getSize().y - 30);
            window_.draw(modeText);
            
            // Add instructions
            sf::Text instructionText("Left Click: Place | ESC: Cancel", font, 14);
            instructionText.setPosition(10, window_.getSize().y - 50);
            instructionText.setFillColor(sf::Color::White);
            window_.draw(instructionText);
        }
    }
    
    // Render UI elements
    uiManager_->render(window_);
    
    // Render info panels
    if (selectedTower_ && showTowerInfo_) {
        renderSelectedTowerInfo();
    }
    
    if (selectedUnit_ && showUnitInfo_) {
        renderSelectedUnitInfo();
    }
    
    renderFloatingTexts();
    
    // Render game state overlays
    if (resourceManager_->hasFont("kenney_mini")) {
        gameStateManager_->renderOverlay(window_, &resourceManager_->getFont("kenney_mini"));
    }
    
    window_.display();
}


// Render tower selection circle
void Game::renderTowerSelection() {
    // Selection circle
    sf::CircleShape selectionCircle(60.0f);
    selectionCircle.setPosition(
        selectedTower_->transform->position.x - 60,
        selectedTower_->transform->position.y - 60
    );
    selectionCircle.setFillColor(sf::Color::Transparent);
    selectionCircle.setOutlineColor(sf::Color(255, 255, 0, 200));
    selectionCircle.setOutlineThickness(3.0f);
    window_.draw(selectionCircle);
    
    // Range circle
    sf::CircleShape rangeCircle(selectedTower_->stats->attackRange);
    rangeCircle.setPosition(
        selectedTower_->transform->position.x - selectedTower_->stats->attackRange,
        selectedTower_->transform->position.y - selectedTower_->stats->attackRange
    );
    rangeCircle.setFillColor(sf::Color(100, 150, 255, 30));
    rangeCircle.setOutlineColor(sf::Color(100, 150, 255, 100));
    rangeCircle.setOutlineThickness(2.0f);
    window_.draw(rangeCircle);
}

// Render unit selection circle
void Game::renderUnitSelection() {
    // Selection circle
    sf::CircleShape selectionCircle(40.0f);
    selectionCircle.setPosition(
        selectedUnit_->transform->position.x - 40,
        selectedUnit_->transform->position.y - 40
    );
    selectionCircle.setFillColor(sf::Color::Transparent);
    selectionCircle.setOutlineColor(sf::Color(0, 255, 0, 200));
    selectionCircle.setOutlineThickness(3.0f);
    window_.draw(selectionCircle);
    
    // Attack range circle
    sf::CircleShape rangeCircle(selectedUnit_->stats->attackRange);
    rangeCircle.setPosition(
        selectedUnit_->transform->position.x - selectedUnit_->stats->attackRange,
        selectedUnit_->transform->position.y - selectedUnit_->stats->attackRange
    );
    rangeCircle.setFillColor(sf::Color(100, 255, 100, 30));
    rangeCircle.setOutlineColor(sf::Color(100, 255, 100, 100));
    rangeCircle.setOutlineThickness(2.0f);
    window_.draw(rangeCircle);
}

// Render tower info panel
void Game::renderSelectedTowerInfo() {
    if (!selectedTower_ || !resourceManager_->hasFont("kenney_mini")) return;
    
    const sf::Font& font = resourceManager_->getFont("kenney_mini");
    
    // Info panel background
    sf::RectangleShape infoPanel(sf::Vector2f(300, 220));
    infoPanel.setPosition(window_.getSize().x - 320, 70);
    infoPanel.setFillColor(sf::Color(0, 0, 0, 200));
    infoPanel.setOutlineColor(sf::Color(255, 255, 0));
    infoPanel.setOutlineThickness(3);
    window_.draw(infoPanel);
    
    // Tower name
    sf::Text nameText;
    nameText.setFont(font);
    nameText.setString(selectedTower_->towerType);
    nameText.setCharacterSize(20);
    nameText.setFillColor(sf::Color::Yellow);
    nameText.setPosition(window_.getSize().x - 310, 80);
    window_.draw(nameText);
    
    // Stats
    float yPos = 110;
    auto drawStat = [&](const std::string& label, const std::string& value) {
        sf::Text statText;
        statText.setFont(font);
        statText.setString(label + ": " + value);
        statText.setCharacterSize(16);
        statText.setFillColor(sf::Color::White);
        statText.setPosition(window_.getSize().x - 310, yPos);
        window_.draw(statText);
        yPos += 22;
    };
    
    drawStat("Level", std::to_string(selectedTower_->upgrade->level));
    drawStat("Damage", std::to_string(static_cast<int>(selectedTower_->stats->damage)));
    drawStat("Range", std::to_string(static_cast<int>(selectedTower_->stats->attackRange)));
    drawStat("Speed", std::to_string(selectedTower_->stats->attackSpeed).substr(0, 4));
    
    // Upgrade info
    if (selectedTower_->upgrade->level < selectedTower_->upgrade->maxLevel) {
        int upgradeCost = UpgradeSystem::getUpgradeCost(selectedTower_->upgrade->level, selectedTower_->towerType);
        sf::Color upgradeColor = (gold_ >= upgradeCost) ? sf::Color::Green : sf::Color::Red;
        drawStat("Upgrade", std::to_string(upgradeCost) + "g");
    } else {
        sf::Text maxText;
        maxText.setFont(font);
        maxText.setString("MAX LEVEL");
        maxText.setCharacterSize(16);
        maxText.setFillColor(sf::Color::Cyan);
        maxText.setPosition(window_.getSize().x - 310, yPos);
        window_.draw(maxText);
        yPos += 22;
    }
    
    // Sell value
    int sellValue = getTowerSellValue(selectedTower_);
    drawStat("Sell", std::to_string(sellValue) + "g");
    
    // Instructions
    sf::Text instructionText;
    instructionText.setFont(font);
    instructionText.setString("E: Upgrade | X/DEL: Sell");
    instructionText.setCharacterSize(14);
    instructionText.setFillColor(sf::Color(200, 200, 200));
    instructionText.setPosition(window_.getSize().x - 310, yPos + 10);
    window_.draw(instructionText);
}

// Render unit info panel - COMPLETED VERSION
void Game::renderSelectedUnitInfo() {
    if (!selectedUnit_ || !resourceManager_->hasFont("kenney_mini")) return;
    
    const sf::Font& font = resourceManager_->getFont("kenney_mini");
    
    // Info panel background
    sf::RectangleShape infoPanel(sf::Vector2f(300, 220));
    infoPanel.setPosition(window_.getSize().x - 320, 70);
    infoPanel.setFillColor(sf::Color(0, 0, 0, 200));
    infoPanel.setOutlineColor(sf::Color(0, 255, 0));
    infoPanel.setOutlineThickness(3);
    window_.draw(infoPanel);
    
    // Unit name
    sf::Text nameText;
    nameText.setFont(font);
    nameText.setString(selectedUnit_->unitType);
    nameText.setCharacterSize(20);
    nameText.setFillColor(sf::Color::Green);
    nameText.setPosition(window_.getSize().x - 310, 80);
    window_.draw(nameText);
    
    // Stats
    float yPos = 110;
    auto drawStat = [&](const std::string& label, const std::string& value) {
        sf::Text statText;
        statText.setFont(font);
        statText.setString(label + ": " + value);
        statText.setCharacterSize(16);
        statText.setFillColor(sf::Color::White);
        statText.setPosition(window_.getSize().x - 310, yPos);
        window_.draw(statText);
        yPos += 22;
    };
    
    drawStat("Level", std::to_string(selectedUnit_->upgrade->level));
    drawStat("HP", std::to_string(selectedUnit_->health->hp) + "/" + std::to_string(selectedUnit_->health->maxHp));
    drawStat("Damage", std::to_string(static_cast<int>(selectedUnit_->stats->damage)));
    drawStat("Speed", std::to_string(static_cast<int>(selectedUnit_->stats->speed)));
    
    // Upgrade info
    if (selectedUnit_->upgrade->level < selectedUnit_->upgrade->maxLevel) {
        int upgradeCost = UpgradeSystem::getUpgradeCost(selectedUnit_->upgrade->level, selectedUnit_->unitType);
        sf::Color upgradeColor = (gold_ >= upgradeCost) ? sf::Color::Green : sf::Color::Red;
        drawStat("Upgrade", std::to_string(upgradeCost) + "g");
    } else {
        sf::Text maxText;
        maxText.setFont(font);
        maxText.setString("MAX LEVEL");
        maxText.setCharacterSize(16);
        maxText.setFillColor(sf::Color::Cyan);
        maxText.setPosition(window_.getSize().x - 310, yPos);
        window_.draw(maxText);
        yPos += 22;
    }
    
    // Instructions
    sf::Text instructionText;
    instructionText.setFont(font);
    instructionText.setString("E: Upgrade | ESC: Deselect");
    instructionText.setCharacterSize(14);
    instructionText.setFillColor(sf::Color(200, 200, 200));
    instructionText.setPosition(window_.getSize().x - 310, yPos + 10);
    window_.draw(instructionText);
}

void Game::renderMap() {
    float mapWidth = map_->cols() * map_->tileSize();
    float mapHeight = map_->rows() * map_->tileSize();
    
    std::string mapTextureId = "map_" + map_->getMapId();
    
    if (resourceManager_->hasTexture(mapTextureId)) {
        sf::Sprite mapSprite;
        mapSprite.setTexture(resourceManager_->getTexture(mapTextureId));
        mapSprite.setPosition(0, 0);
        window_.draw(mapSprite);
    } else {
        // Fallback grid
        for (int y = 0; y < map_->rows(); ++y) {
            for (int x = 0; x < map_->cols(); ++x) {
                sf::RectangleShape tile(sf::Vector2f(map_->tileSize(), map_->tileSize()));
                tile.setPosition(x * map_->tileSize(), y * map_->tileSize());
                
                int index = y * map_->cols() + x;
                if (index < static_cast<int>(map_->grid().size())) {
                    tile.setFillColor(map_->grid()[index] ? sf::Color(100, 100, 100) : sf::Color(50, 150, 50));
                }
                tile.setOutlineColor(sf::Color(40, 120, 40, 100));
                tile.setOutlineThickness(1.0f);
                window_.draw(tile);
            }
        }
    }
    
    // Draw path
    if (map_->getPath().size() > 1) {
        for (size_t i = 0; i < map_->getPath().size() - 1; ++i) {
            sf::Vertex line[] = {
                sf::Vertex(map_->getPath()[i], sf::Color(255, 200, 100, 180)),
                sf::Vertex(map_->getPath()[i + 1], sf::Color(255, 200, 100, 180))
            };
            window_.draw(line, 2, sf::Lines);
        }
        
        for (const auto& point : map_->getPath()) {
            sf::CircleShape waypoint(5.0f);
            waypoint.setPosition(point.x - 5, point.y - 5);
            waypoint.setFillColor(sf::Color(255, 150, 0, 200));
            waypoint.setOutlineColor(sf::Color::Yellow);
            waypoint.setOutlineThickness(1.0f);
            window_.draw(waypoint);
        }
    }
    
    if (debugMode_) {
        for (int y = 0; y < map_->rows(); ++y) {
            for (int x = 0; x < map_->cols(); ++x) {
                int index = y * map_->cols() + x;
                if (index < static_cast<int>(map_->grid().size())) {
                    sf::RectangleShape gridCell(sf::Vector2f(map_->tileSize() - 2, map_->tileSize() - 2));
                    gridCell.setPosition(x * map_->tileSize() + 1, y * map_->tileSize() + 1);
                    gridCell.setFillColor(sf::Color::Transparent);
                    
                    if (map_->grid()[index] == 0) {
                        gridCell.setOutlineColor(sf::Color(255, 0, 0, 100));
                    } else {
                        gridCell.setOutlineColor(sf::Color(0, 255, 0, 50));
                    }
                    gridCell.setOutlineThickness(1.0f);
                    window_.draw(gridCell);
                }
            }
        }
    }
}

void Game::renderTowers() {
    for (const auto& tower : towerSystem_->getTowers()) {
        if (tower->sprite && tower->sprite->visible) {
            if (tower->sprite->textureId == "MAIN_BASE") {
                // UNIQUE MAIN BASE TOWER - Black fortress
                sf::RectangleShape baseTower(sf::Vector2f(100.0f, 100.0f));
                baseTower.setPosition(tower->transform->position.x - 50, tower->transform->position.y - 50);
                baseTower.setFillColor(sf::Color(20, 20, 20));
                baseTower.setOutlineColor(sf::Color(150, 150, 150));
                baseTower.setOutlineThickness(3.0f);
                window_.draw(baseTower);
                
                // Top battlements
                for (int i = 0; i < 4; i++) {
                    sf::RectangleShape battlement(sf::Vector2f(15.0f, 15.0f));
                    battlement.setPosition(tower->transform->position.x - 50 + (i * 28), tower->transform->position.y - 58);
                    battlement.setFillColor(sf::Color(40, 40, 40));
                    battlement.setOutlineColor(sf::Color(100, 100, 100));
                    battlement.setOutlineThickness(1.0f);
                    window_.draw(battlement);
                }
                
                // Central core
                sf::CircleShape core(20.0f);
                core.setPosition(tower->transform->position.x - 20, tower->transform->position.y - 20);
                core.setFillColor(sf::Color(255, 50, 50, 180));
                core.setOutlineColor(sf::Color(255, 100, 100));
                core.setOutlineThickness(2.0f);
                window_.draw(core);
                
                // Base platform
                sf::RectangleShape platform(sf::Vector2f(110.0f, 10.0f));
                platform.setPosition(tower->transform->position.x - 55, tower->transform->position.y + 50);
                platform.setFillColor(sf::Color(60, 60, 60));
                platform.setOutlineColor(sf::Color(100, 100, 100));
                platform.setOutlineThickness(1.0f);
                window_.draw(platform);
            } else if (tower->sprite->sprite.getTexture()) {
                tower->sprite->sprite.setPosition(tower->transform->position);
                tower->sprite->sprite.setScale(0.8f, 0.8f);
                window_.draw(tower->sprite->sprite);
            }
            
            if (debugMode_) {
                sf::CircleShape rangeCircle(tower->stats->attackRange);
                rangeCircle.setPosition(
                    tower->transform->position.x - tower->stats->attackRange,
                    tower->transform->position.y - tower->stats->attackRange
                );
                rangeCircle.setFillColor(sf::Color::Transparent);
                rangeCircle.setOutlineColor(sf::Color(100, 150, 255, 60));
                rangeCircle.setOutlineThickness(2.0f);
                window_.draw(rangeCircle);
            }
        }
    }
}

void Game::renderUnits() {
    for (const auto& unit : unitSystem_->getUnits()) {
        if (unit->sprite && unit->sprite->visible && unit->health->alive()) {
            if (unit->sprite->sprite.getTexture()) {
                unit->sprite->sprite.setPosition(unit->transform->position);
                unit->sprite->sprite.setScale(0.2f, 0.2f);
                window_.draw(unit->sprite->sprite);
            }
            
            renderHealthBar(
                sf::Vector2f(unit->transform->position.x - 15, unit->transform->position.y - 25),
                static_cast<float>(unit->health->hp) / unit->health->maxHp,
                30.0f, 4.0f
            );
        }
    }
}

void Game::renderEnemies() {
    for (const auto& enemy : enemySystem_->getEnemies()) {
        if (enemy->sprite && enemy->sprite->visible && enemy->health->alive()) {
            if (enemy->sprite->sprite.getTexture()) {
                enemy->sprite->sprite.setPosition(enemy->transform->position);
                enemy->sprite->sprite.setScale(0.2f, 0.2f);
                window_.draw(enemy->sprite->sprite);
            }
            
            renderHealthBar(
                sf::Vector2f(enemy->transform->position.x - 15, enemy->transform->position.y - 25),
                static_cast<float>(enemy->health->hp) / enemy->health->maxHp,
                30.0f, 4.0f
            );
        }
    }
}

void Game::renderProjectiles() {
    for (const auto& proj : projectileSystem_->getProjectiles()) {
        if (proj.active) {
            sf::CircleShape projShape(4.0f);
            projShape.setPosition(proj.pos.x - 4, proj.pos.y - 4);
            
            switch (proj.type) {
                case ProjectileData::FIREBALL:
                    projShape.setFillColor(sf::Color(255, 150, 50));
                    break;
                case ProjectileData::ICE_SHARD:
                    projShape.setFillColor(sf::Color(100, 200, 255));
                    break;
                case ProjectileData::POISON_DART:
                    projShape.setFillColor(sf::Color(100, 255, 100));
                    break;
                case ProjectileData::LIGHTNING:
                    projShape.setFillColor(sf::Color(255, 255, 100));
                    break;
                default:
                    projShape.setFillColor(sf::Color::Yellow);
                    break;
            }
            
            window_.draw(projShape);
        }
    }
}

void Game::renderParticles() {
    for (const auto& particle : particleSystem_->getParticles()) {
        if (particle.alive) {
            sf::CircleShape shape(particle.size);
            shape.setPosition(particle.pos.x - particle.size, particle.pos.y - particle.size);
            
            float t = 1.0f - (particle.life / particle.maxLife);
            sf::Color color;
            color.r = static_cast<sf::Uint8>(particle.startColor.r * (1-t) + particle.endColor.r * t);
            color.g = static_cast<sf::Uint8>(particle.startColor.g * (1-t) + particle.endColor.g * t);
            color.b = static_cast<sf::Uint8>(particle.startColor.b * (1-t) + particle.endColor.b * t);
            color.a = static_cast<sf::Uint8>(particle.startColor.a * (1-t) + particle.endColor.a * t);
            
            shape.setFillColor(color);
            window_.draw(shape);
        }
    }
}

void Game::renderHealthBar(const sf::Vector2f& position, float healthPercent, float width, float height) {
    sf::RectangleShape background(sf::Vector2f(width, height));
    background.setFillColor(sf::Color::Red);
    background.setPosition(position);
    
    sf::RectangleShape health(sf::Vector2f(width * healthPercent, height));
    health.setFillColor(sf::Color::Green);
    health.setPosition(position);
    
    window_.draw(background);
    window_.draw(health);
}

void Game::renderTowerPlacementPreview() {
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window_);
    sf::Vector2f mousePos = window_.mapPixelToCoords(mousePixel);
    
    bool validPosition = isValidTowerPosition(mousePos);
    int cost = getTowerCost(selectedTowerType_);
    bool canAfford = (gold_ >= cost);
    
    sf::CircleShape preview(25.0f);
    preview.setPosition(mousePos.x - 25, mousePos.y - 25);
    
    if (validPosition && canAfford) {
        preview.setFillColor(sf::Color(100, 255, 100, 100));
        preview.setOutlineColor(sf::Color::Green);
    } else {
        preview.setFillColor(sf::Color(255, 100, 100, 100));
        preview.setOutlineColor(sf::Color::Red);
    }
    preview.setOutlineThickness(3.0f);
    window_.draw(preview);
    
    int range = 200;
    if (selectedTowerType_ == "cannon_tower") range = 180;
    if (selectedTowerType_ == "mage_tower") range = 250;
    
    sf::CircleShape rangePreview(range);
    rangePreview.setPosition(mousePos.x - range, mousePos.y - range);
    rangePreview.setFillColor(sf::Color::Transparent);
    rangePreview.setOutlineColor(sf::Color(100, 150, 255, 120));
    rangePreview.setOutlineThickness(2.0f);
    window_.draw(rangePreview);
}

// NEW: Unit placement preview
void Game::renderUnitPlacementPreview() {
    sf::Vector2i mousePixel = sf::Mouse::getPosition(window_);
    sf::Vector2f mousePos = window_.mapPixelToCoords(mousePixel);
    
    bool validPosition = isValidUnitPosition(mousePos);
    int cost = getUnitCost(selectedUnitType_);
    bool canAfford = (gold_ >= cost);
    
    sf::CircleShape preview(20.0f);
    preview.setPosition(mousePos.x - 20, mousePos.y - 20);
    
    if (validPosition && canAfford) {
        preview.setFillColor(sf::Color(100, 255, 100, 100));
        preview.setOutlineColor(sf::Color::Green);
    } else {
        preview.setFillColor(sf::Color(255, 100, 100, 100));
        preview.setOutlineColor(sf::Color::Red);
    }
    preview.setOutlineThickness(3.0f);
    window_.draw(preview);
}

void Game::renderFloatingTexts() {
    if (resourceManager_->hasFont("kenney_mini")) {
        const sf::Font& font = resourceManager_->getFont("kenney_mini");
        for (const auto& floatingText : floatingTexts_) {
            if (floatingText.active) {
                sf::Text text;
                text.setFont(font);
                text.setString(floatingText.text);
                text.setCharacterSize(18);
                text.setFillColor(floatingText.color);
                text.setPosition(floatingText.position);
                window_.draw(text);
            }
        }
    }
}

void Game::updateWaveSystem(float dt) {
    if (nextWaveTimer_ > 0.0f) {
        nextWaveTimer_ -= dt;
        if (nextWaveTimer_ < 0.0f) nextWaveTimer_ = 0.0f;
    }
    
    if (!waveInProgress_) {
        return;
    }
    
    if (waveSystem_->isActive()) {
        waveSystem_->update(dt);
        
        if (waveSystem_->isFinished() && enemySystem_->getAliveCount() == 0) {
            std::cout << "[Game] ========================================" << std::endl;
            std::cout << "[Game] WAVE " << (currentWave_ + 1) << " COMPLETED!" << std::endl;
            std::cout << "[Game] ========================================" << std::endl;
            
            waveInProgress_ = false;
            
            int reward = 50 + (currentWave_ * 25);
            gold_ += reward;
            std::cout << "[Game] Reward: " << reward << " gold (Total: " << gold_ << ")" << std::endl;
            
            WaveEventData data;
            data.waveNumber = currentWave_ + 1;
            data.goldReward = reward;
            eventBus_->publish(Events::WAVE_COMPLETED, &data);
            
            currentWave_++;
            
            if (currentWave_ >= waveSystem_->getTotalWaves()) {
                std::cout << "[Game] ✓✓✓ ALL WAVES COMPLETED - VICTORY! ✓✓✓" << std::endl;
                eventBus_->publish(Events::ALL_WAVES_COMPLETED, nullptr);
                gameStateManager_->victory();
            } else {
                nextWaveTimer_ = 5.0f;
                std::cout << "[Game] Next wave ready in " << nextWaveTimer_ << " seconds" << std::endl;
            }
        }
    }
}

void Game::updateEntities(float dt) {
    for (const auto& enemy : enemySystem_->getEnemies()) {
        enemy->update(dt);
        if (enemy->sprite) {
            enemy->sprite->sprite.setPosition(enemy->transform->position);
        }
    }
    enemySystem_->update(dt);
    
    for (const auto& unit : unitSystem_->getUnits()) {
        unit->update(dt);
        if (unit->sprite) {
            unit->sprite->sprite.setPosition(unit->transform->position);
        }
    }
    unitSystem_->update(dt);
    
    for (const auto& tower : towerSystem_->getTowers()) {
        tower->update(dt);
    }
    towerSystem_->update(dt);
    
    projectileSystem_->update(dt);
}

void Game::updateUI() {
    uiManager_->setResources(gold_, lives_, 0);
    uiManager_->setWaveInfo(currentWave_ + 1, waveSystem_->getTotalWaves(), 
                            enemySystem_->getAliveCount(), nextWaveTimer_);
    
    if (!waveInProgress_ && nextWaveTimer_ <= 0.0f && currentWave_ < waveSystem_->getTotalWaves()) {
        uiManager_->showWaveReady(true);
    } else {
        uiManager_->showWaveReady(false);
    }
}

void Game::createInitialTowers() {
    // Create UNIQUE main base tower
    mainBaseTower_ = std::make_shared<Tower>();
    mainBaseTower_->initializeAsType("arrow_tower");
    mainBaseTower_->transform->position = sf::Vector2f(150, 350);
    mainBaseTower_->sprite->visible = true;
    mainBaseTower_->sprite->textureId = "MAIN_BASE";
    towerSystem_->add(mainBaseTower_);
    
    std::cout << "[Game] Created MAIN BASE tower at (150, 350)" << std::endl;
}

void Game::spawnEnemy(const std::string& enemyId) {
    auto enemy = std::make_shared<Enemy>();
    enemy->initializeAsType(enemyId);
    
    enemy->path->path = map_->getPath();
    enemy->path->currentIndex = 0;
    enemy->path->finished = false;
    
    if (!enemy->path->path.empty()) {
        enemy->transform->position = enemy->path->path[0];
    }
    
    std::string textureId = "enemy_" + enemyId;
    if (resourceManager_->hasTexture(textureId)) {
        enemy->sprite->sprite.setTexture(resourceManager_->getTexture(textureId));
        enemy->sprite->sprite.setOrigin(16, 16);
        enemy->sprite->visible = true;
    }
    
    enemySystem_->add(enemy);
}

void Game::handleMouseClickWorld(const sf::Vector2f& worldPos) {
    if (!gameStateManager_->isPlaying()) return; // Don't allow placement when paused/game over
    
    if (placingTower_) {
        std::cout << "[DEBUG] Placing TOWER: " << selectedTowerType_ << std::endl;
        if (isValidTowerPosition(worldPos)) {
            int cost = getTowerCost(selectedTowerType_);
            if (gold_ >= cost) {
                if (placeTower(selectedTowerType_, worldPos)) {
                    std::cout << "[Game] Tower placed at (" << worldPos.x << ", " << worldPos.y << ")" << std::endl;
                    placingTower_ = false;
                    selectedTowerType_.clear();
                } else {
                    spawnFloatingText("Failed to place!", worldPos, sf::Color::Red);
                }
            } else {
                std::cout << "[Game] NOT ENOUGH GOLD! Need: " << cost << ", Have: " << gold_ << std::endl;
                spawnFloatingText("Not enough gold!", worldPos, sf::Color::Red);
            }
        } else {
            spawnFloatingText("Cannot place here!", worldPos, sf::Color::Red);
        }
    } else if (placingUnit_) {
        std::cout << "[DEBUG] Placing UNIT: " << selectedUnitType_ << std::endl;
        if (isValidUnitPosition(worldPos)) {
            int cost = getUnitCost(selectedUnitType_);
            if (gold_ >= cost) {
                if (placeUnit(selectedUnitType_, worldPos)) {
                    std::cout << "[Game] Unit placed at (" << worldPos.x << ", " << worldPos.y << ")" << std::endl;
                    placingUnit_ = false;
                    selectedUnitType_.clear();
                } else {
                    spawnFloatingText("Failed to place!", worldPos, sf::Color::Red);
                }
            } else {
                std::cout << "[Game] NOT ENOUGH GOLD! Need: " << cost << ", Have: " << gold_ << std::endl;
                spawnFloatingText("Not enough gold!", worldPos, sf::Color::Red);
            }
        } else {
            spawnFloatingText("Cannot place here!", worldPos, sf::Color::Red);
        }
    } else {
        // Try to select something
        selectEntity(worldPos);
    }
}

// NEW: Select tower or unit
void Game::selectEntity(const sf::Vector2f& worldPos) {
    // Try to select tower first
    auto tower = getTowerAtPosition(worldPos);
    if (tower) {
        if (selectedTower_) {
            // Deselect previous tower
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
        }
        if (selectedUnit_) {
            // Deselect unit if selecting tower
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
        }
        
        selectedTower_ = tower;
        showTowerInfo_ = true;
        
        std::cout << "[Game] Selected " << tower->towerType << std::endl;
        particleSystem_->emit(tower->transform->position, Particle::SPARKLE, 5);
        return;
    }
    
    // Try to select unit
    auto unit = getUnitAtPosition(worldPos);
    if (unit) {
        if (selectedTower_) {
            // Deselect tower if selecting unit
            selectedTower_ = nullptr;
            showTowerInfo_ = false;
        }
        if (selectedUnit_) {
            // Deselect previous unit
            selectedUnit_ = nullptr;
            showUnitInfo_ = false;
        }
        
        selectedUnit_ = unit;
        showUnitInfo_ = true;
        
        std::cout << "[Game] Selected " << unit->unitType << std::endl;
        particleSystem_->emit(unit->transform->position, Particle::SPARKLE, 5);
        return;
    }
    
    // Nothing selected
    deselectAll();
}

void Game::deselectTower() {
    if (selectedTower_) {
        std::cout << "[Game] Deselected tower" << std::endl;
    }
    selectedTower_ = nullptr;
    showTowerInfo_ = false;
}

void Game::deselectUnit() {
    if (selectedUnit_) {
        std::cout << "[Game] Deselected unit" << std::endl;
    }
    selectedUnit_ = nullptr;
    showUnitInfo_ = false;
}

void Game::deselectAll() {
    placingTower_ = false;
    placingUnit_ = false;
    selectedTowerType_.clear();
    selectedUnitType_.clear();
    
    if (selectedTower_) {
        std::cout << "[Game] Deselected tower" << std::endl;
        selectedTower_ = nullptr;
        showTowerInfo_ = false;
    }
    
    if (selectedUnit_) {
        std::cout << "[Game] Deselected unit" << std::endl;
        selectedUnit_ = nullptr;
        showUnitInfo_ = false;
    }
}


std::shared_ptr<Tower> Game::getTowerAtPosition(const sf::Vector2f& worldPos) {
    for (const auto& tower : towerSystem_->getTowers()) {
        sf::Vector2f diff = tower->transform->position - worldPos;
        float distanceSq = diff.x * diff.x + diff.y * diff.y;
        float radiusSq = 50.0f * 50.0f;
        
        if (distanceSq <= radiusSq) {
            return tower;
        }
    }
    return nullptr;
}

std::shared_ptr<Unit> Game::getUnitAtPosition(const sf::Vector2f& worldPos) {
    for (const auto& unit : unitSystem_->getUnits()) {
        if (!unit->health->alive()) continue;
        
        sf::Vector2f diff = unit->transform->position - worldPos;
        float distanceSq = diff.x * diff.x + diff.y * diff.y;
        float radiusSq = 30.0f * 30.0f;
        
        if (distanceSq <= radiusSq) {
            return unit;
        }
    }
    return nullptr;
}

int Game::getTowerCost(const std::string& towerType) {
    if (towerType == "arrow_tower") return 100;
    if (towerType == "cannon_tower") return 200;
    if (towerType == "mage_tower") return 300;
    if (towerType == "ice_tower") return 250;
    if (towerType == "lightning_tower") return 400;
    if (towerType == "poison_tower") return 350;
    if (towerType == "ballista") return 450;
    if (towerType == "flame_tower") return 500;
    if (towerType == "tesla_tower") return 600;
    if (towerType == "arcane_tower") return 800;
    if (towerType == "sniper_tower") return 700;
    if (towerType == "artillery_tower") return 1000;
    return 100;
}

int Game::getUnitCost(const std::string& unitType) {
    if (unitType == "archer") return 80;
    if (unitType == "knight") return 120;
    if (unitType == "mage") return 150;
    if (unitType == "rogue") return 100;
    if (unitType == "paladin") return 180;
    if (unitType == "ranger") return 130;
    if (unitType == "berserker") return 160;
    if (unitType == "priest") return 140;
    if (unitType == "necromancer") return 170;
    if (unitType == "druid") return 190;
    if (unitType == "engineer") return 210;
    if (unitType == "monk") return 220;
    return 80;
}

int Game::getTowerSellValue(const std::shared_ptr<Tower>& tower) {
    int baseCost = getTowerCost(tower->towerType);
    
    int upgradeCosts = 0;
    if (tower->upgrade && tower->upgrade->level > 1) {
        for (int i = 1; i < tower->upgrade->level; i++) {
            upgradeCosts += baseCost * (i + 1);
        }
    }
    
    return static_cast<int>((baseCost + upgradeCosts) * 0.75f);
}

void Game::sellSelectedTower() {
    if (!selectedTower_) return;
    
    if (selectedTower_ == mainBaseTower_ || selectedTower_->sprite->textureId == "MAIN_BASE") {
        std::cout << "[Game] Cannot sell the MAIN BASE tower!" << std::endl;
        spawnFloatingText("Cannot sell base!", selectedTower_->transform->position, sf::Color::Red);
        return;
    }
    
    int refund = getTowerSellValue(selectedTower_);
    gold_ += refund;
    
    std::cout << "[Game] Sold " << selectedTower_->towerType << " for " << refund << " gold (Total: " << gold_ << ")" << std::endl;
    
    particleSystem_->emitExplosion(selectedTower_->transform->position, 30.0f);
    showGoldText(refund, selectedTower_->transform->position);
    spawnFloatingText("SOLD!", selectedTower_->transform->position, sf::Color::Yellow);
    
    auto& towers = towerSystem_->getTowersModifiable();
    towers.erase(std::remove(towers.begin(), towers.end(), selectedTower_), towers.end());
    
    deselectTower();
}

// NEW: Upgrade selected tower
void Game::upgradeSelectedTower() {
    if (!selectedTower_) return;
    
    if (selectedTower_->upgrade->level >= selectedTower_->upgrade->maxLevel) {
        std::cout << "[Game] Tower already at max level!" << std::endl;
        spawnFloatingText("MAX LEVEL!", selectedTower_->transform->position, sf::Color::Cyan);
        return;
    }
    
    if (UpgradeSystem::upgradeTower(selectedTower_, gold_)) {
        std::cout << "[Game] Upgraded " << selectedTower_->towerType 
                  << " to level " << selectedTower_->upgrade->level 
                  << " (Gold: " << gold_ << ")" << std::endl;
        
        particleSystem_->emit(selectedTower_->transform->position, Particle::SPARKLE, 20);
        spawnFloatingText("UPGRADED!", selectedTower_->transform->position, sf::Color::Cyan);
    } else {
        int cost = UpgradeSystem::getUpgradeCost(selectedTower_->upgrade->level - 1, selectedTower_->towerType);
        std::cout << "[Game] Cannot upgrade! Need: " << cost << "g, Have: " << gold_ << "g" << std::endl;
        spawnFloatingText("Not enough gold!", selectedTower_->transform->position, sf::Color::Red);
    }
}

// NEW: Upgrade selected unit
void Game::upgradeSelectedUnit() {
    if (!selectedUnit_) return;
    
    if (selectedUnit_->upgrade->level >= selectedUnit_->upgrade->maxLevel) {
        std::cout << "[Game] Unit already at max level!" << std::endl;
        spawnFloatingText("MAX LEVEL!", selectedUnit_->transform->position, sf::Color::Cyan);
        return;
    }
    
    if (UpgradeSystem::upgradeUnit(selectedUnit_, gold_)) {
        std::cout << "[Game] Upgraded " << selectedUnit_->unitType 
                  << " to level " << selectedUnit_->upgrade->level 
                  << " (Gold: " << gold_ << ")" << std::endl;
        
        particleSystem_->emit(selectedUnit_->transform->position, Particle::SPARKLE, 20);
        spawnFloatingText("UPGRADED!", selectedUnit_->transform->position, sf::Color::Cyan);
    } else {
        int cost = UpgradeSystem::getUpgradeCost(selectedUnit_->upgrade->level - 1, selectedUnit_->unitType);
        std::cout << "[Game] Cannot upgrade! Need: " << cost << "g, Have: " << gold_ << "g" << std::endl;
        spawnFloatingText("Not enough gold!", selectedUnit_->transform->position, sf::Color::Red);
    }
}

bool Game::isValidTowerPosition(const sf::Vector2f& position) {
    float mapWidth = map_->cols() * map_->tileSize();
    float mapHeight = map_->rows() * map_->tileSize();
    
    if (position.x < 0 || position.x > mapWidth || 
        position.y < 0 || position.y > mapHeight) {
        return false;
    }
    
    bool buildable = map_->isValidBuildPosition(position);
    if (!buildable) {
        return false;
    }
    
    for (const auto& tower : towerSystem_->getTowers()) {
        float dx = position.x - tower->transform->position.x;
        float dy = position.y - tower->transform->position.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        if (distance < 60.0f) {
            return false;
        }
    }
    
    float minPathDistance = 999999.0f;
    for (const auto& pathPoint : map_->getPath()) {
        float dx = position.x - pathPoint.x;
        float dy = position.y - pathPoint.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        if (distance < minPathDistance) {
            minPathDistance = distance;
        }
    }
    
    if (minPathDistance < 20.0f) {
        return false;
    }
    
    return true;
}

// NEW: Validate unit position
bool Game::isValidUnitPosition(const sf::Vector2f& position) {
    float mapWidth = map_->cols() * map_->tileSize();
    float mapHeight = map_->rows() * map_->tileSize();
    
    // Must be within map bounds
    if (position.x < 0 || position.x > mapWidth || 
        position.y < 0 || position.y > mapHeight) {
        return false;
    }
    
    // Cannot be on path (must be on buildable ground)
    bool buildable = map_->isValidBuildPosition(position);
    if (!buildable) {
        return false;
    }
    
    // Cannot be too close to towers
    for (const auto& tower : towerSystem_->getTowers()) {
        float dx = position.x - tower->transform->position.x;
        float dy = position.y - tower->transform->position.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        if (distance < 50.0f) {
            return false;
        }
    }
    
    // Cannot be too close to other units
    for (const auto& unit : unitSystem_->getUnits()) {
        float dx = position.x - unit->transform->position.x;
        float dy = position.y - unit->transform->position.y;
        float distance = std::sqrt(dx * dx + dy * dy);
        if (distance < 30.0f) {
            return false;
        }
    }
    
    return true;
}

bool Game::placeTower(const std::string& towerType, const sf::Vector2f& position) {
    if (!isValidTowerPosition(position)) {
        return false;
    }
    
    int cost = getTowerCost(towerType);
    
    if (gold_ < cost) {
        std::cout << "[Game] Not enough gold. Need: " << cost << ", Have: " << gold_ << std::endl;
        return false;
    }
    
    if (!towerSystem_->canPlaceTower(position)) {
        return false;
    }
    
    auto tower = std::make_shared<Tower>();
    tower->initializeAsType(towerType);
    tower->transform->position = position;
    
    std::string textureId = "tower_" + towerType;
    size_t pos = textureId.find("_tower");
    if (pos != std::string::npos) {
        textureId.erase(pos, 6);
    }
    
    if (resourceManager_->hasTexture(textureId)) {
        tower->sprite->sprite.setTexture(resourceManager_->getTexture(textureId));
        tower->sprite->sprite.setOrigin(32, 32);
        tower->sprite->visible = true;
    }
    
    towerSystem_->add(tower);
    
    // CRITICAL FIX: Deduct gold AFTER adding tower
    gold_ -= cost;
    std::cout << "[Game] ✓ TOWER PLACED - Deducted " << cost << " gold. Remaining: " << gold_ << std::endl;
    
    TowerEventData data;
    data.tower = tower;
    data.position = position;
    data.towerType = towerType;
    data.cost = cost;
    eventBus_->publish(Events::TOWER_PLACED, &data);
    
    return true;
}

// NEW: Place unit
bool Game::placeUnit(const std::string& unitType, const sf::Vector2f& position) {
    if (!isValidUnitPosition(position)) {
        return false;
    }
    
    int cost = getUnitCost(unitType);
    
    if (gold_ < cost) {
        std::cout << "[Game] Not enough gold. Need: " << cost << ", Have: " << gold_ << std::endl;
        return false;
    }
    
    auto unit = std::make_shared<Unit>();
    unit->initializeAsType(unitType);
    unit->transform->position = position;
    
    std::string textureId = "unit_" + unitType;
    if (resourceManager_->hasTexture(textureId)) {
        unit->sprite->sprite.setTexture(resourceManager_->getTexture(textureId));
        unit->sprite->sprite.setOrigin(16, 16);
        unit->sprite->visible = true;
    }
    
    unitSystem_->add(unit);
    
    // CRITICAL FIX: Deduct gold AFTER adding unit
    gold_ -= cost;
    std::cout << "[Game] ✓ UNIT PLACED - Deducted " << cost << " gold. Remaining: " << gold_ << std::endl;
    
    particleSystem_->emitExplosion(position, 15.0f);
    
    return true;
}

void Game::startNextWave() {
    if (currentWave_ < waveSystem_->getTotalWaves() && !waveInProgress_) {
        waveSystem_->start(currentWave_);
        waveInProgress_ = true;
        nextWaveTimer_ = 0.0f;
        std::cout << "[Game] ========================================" << std::endl;
        std::cout << "[Game] Starting wave " << (currentWave_ + 1) << " of " << waveSystem_->getTotalWaves() << std::endl;
        std::cout << "[Game] ========================================" << std::endl;
    }
}

void Game::restartGame() {
    std::cout << "[Game] ========================================" << std::endl;
    std::cout << "[Game] RESTARTING GAME" << std::endl;
    std::cout << "[Game] ========================================" << std::endl;
    
    towerSystem_->getTowersModifiable().clear();
    unitSystem_->getUnitsModifiable().clear();
    projectileSystem_->clear();
    particleSystem_->clear();
    floatingTexts_.clear();
    
    auto& enemies = enemySystem_->getEnemies();
    for (auto& enemy : enemies) {
        if (enemy && enemy->health) {
            enemy->health->hp = 0;
        }
    }
    enemySystem_->removeDead();
    
    gold_ = 500;
    lives_ = 20;
    currentWave_ = 0;
    nextWaveTimer_ = 0.0f;
    waveInProgress_ = false;
    placingTower_ = false;
    placingUnit_ = false;
    selectedTowerType_.clear();
    selectedUnitType_.clear();
    deselectAll();
    
    gameStateManager_->restart();
    
    createInitialTowers();
    setupEntityTextures();
    
    std::cout << "[Game] Game restarted successfully!" << std::endl;
    std::cout << "[Game] Gold: " << gold_ << ", Lives: " << lives_ << ", Wave: " << currentWave_ << std::endl;
}

void Game::applyScreenShake(float intensity, float duration) {
    screenShakeIntensity_ = intensity;
    screenShakeTimer_ = duration;
}

void Game::updateScreenEffects(float dt) {
    if (screenShakeTimer_ > 0.0f) {
        screenShakeTimer_ -= dt;
        
        float shake = screenShakeIntensity_ * (screenShakeTimer_ / 0.5f);
        cameraOffset_.x = (std::rand() % 100 - 50) / 50.0f * shake;
        cameraOffset_.y = (std::rand() % 100 - 50) / 50.0f * shake;
    } else {
        cameraOffset_ = sf::Vector2f(0, 0);
    }
}

void Game::updateFloatingTexts(float dt) {
    for (auto& text : floatingTexts_) {
        if (text.active) {
            text.lifetime -= dt;
            text.position.y += text.velocityY * dt;
            text.velocityY -= 50.0f * dt;
            
            if (text.lifetime <= 0.0f) {
                text.active = false;
            }
            
            float alpha = (text.lifetime / text.maxLifetime) * 255.0f;
            text.color.a = static_cast<sf::Uint8>(alpha);
        }
    }
    
    floatingTexts_.erase(
        std::remove_if(floatingTexts_.begin(), floatingTexts_.end(),
            [](const FloatingText& t) { return !t.active; }),
        floatingTexts_.end()
    );
}

void Game::spawnFloatingText(const std::string& text, const sf::Vector2f& position, const sf::Color& color) {
    FloatingText ft;
    ft.text = text;
    ft.position = position;
    ft.color = color;
    ft.lifetime = 1.5f;
    ft.maxLifetime = 1.5f;
    ft.velocityY = -50.0f;
    ft.active = true;
    
    floatingTexts_.push_back(ft);
}

void Game::setupEntityAnimations() {
    animationSystemEnabled_ = true;
}

void Game::triggerScreenShake(float intensity, float duration) {
    applyScreenShake(intensity, duration);
}

void Game::showDamageNumber(int damage, const sf::Vector2f& position) {
    spawnFloatingText("-" + std::to_string(damage), position, sf::Color::Red);
}

void Game::showGoldText(int gold, const sf::Vector2f& position) {
    spawnFloatingText("+" + std::to_string(gold) + "g", position, sf::Color::Yellow);
}

void Game::quickSave() {
    GameSaveData saveData;
    saveData.gold = gold_;
    saveData.lives = lives_;
    saveData.currentWave = currentWave_;
    saveData.score = 0;
    saveData.nextWaveTimer = nextWaveTimer_;
    saveData.waveInProgress = waveInProgress_;
    saveData.currentMap = "forest_path";
    
    std::vector<std::shared_ptr<Tower>> towersCopy;
    for (const auto& tower : towerSystem_->getTowers()) {
        towersCopy.push_back(tower);
    }
    
    std::vector<std::shared_ptr<Unit>> unitsCopy;
    for (const auto& unit : unitSystem_->getUnits()) {
        unitsCopy.push_back(unit);
    }
    
    if (SaveLoadSystem::saveGame("quicksave.json", saveData, towersCopy, unitsCopy)) {
        std::cout << "[Game] Quick save successful!" << std::endl;
        spawnFloatingText("Game Saved!", sf::Vector2f(320, 100), sf::Color::Cyan);
    }
}

void Game::quickLoad() {
    GameSaveData saveData;
    std::vector<std::shared_ptr<Tower>> towers;
    std::vector<std::shared_ptr<Unit>> units;
    
    if (SaveLoadSystem::loadGame("quicksave.json", saveData, towers, units)) {
        gold_ = saveData.gold;
        lives_ = saveData.lives;
        currentWave_ = saveData.currentWave;
        nextWaveTimer_ = saveData.nextWaveTimer;
        waveInProgress_ = saveData.waveInProgress;
        
        auto& enemies = enemySystem_->getEnemies();
        for (auto& enemy : enemies) {
            if (enemy && enemy->health) {
                enemy->health->hp = 0;
            }
        }
        enemySystem_->removeDead();
        
        towerSystem_->getTowersModifiable().clear();
        for (const auto& tower : towers) {
            towerSystem_->add(tower);
        }

        unitSystem_->getUnitsModifiable().clear();
        for (const auto& unit : units) {
            unitSystem_->add(unit);
        }
        
        deselectAll();
        setupEntityTextures();
        std::cout << "[Game] Quick load successful!" << std::endl;
        spawnFloatingText("Game Loaded!", sf::Vector2f(320, 100), sf::Color::Cyan);
    }
}
// Add these corrected implementations to your Game.cpp file

// Add these corrected implementations to your Game.cpp file

int Game::run() {
    while (running_ && window_.isOpen()) {
        sf::Time elapsed = clock_.restart();
        float dt = elapsed.asSeconds();
        
        // Cap delta time to prevent spiral of death
        if (dt > 0.1f) dt = 0.1f;
        
        // Process events
        sf::Event event;
        while (window_.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window_.close();
                running_ = false;
            }
            
            // Pass events to input handler
            inputHandler_->handleEvent(event);
        }
        
        // Update camera input (NO dt parameter)
        if (gameStateManager_->isPlaying() || gameStateManager_->isPaused()) {
            inputHandler_->update(); // FIXED: No parameter
            cameraSystem_->update(dt);
        }
        
        // Update game
        if (gameStateManager_->isPlaying()) {
            updateEntities(dt);
            updateWaveSystem(dt);
            updateScreenEffects(dt);
            updateFloatingTexts(dt);
            collisionSystem_->update(dt);
            particleSystem_->update(dt);
            
            // AnimationSystem update is handled per-entity, not globally
            // Remove the global animationSystem_->update(dt) call
        }
        
        updateUI();
        
        // Render
        render();
    }
    
    return 0;
}

void Game::setupEventSubscriptions() {
    std::cout << "[Game] Setting up event subscriptions..." << std::endl;
    
    // Enemy death - FIXED: EventBus callback signature requires (topic, data)
    eventBus_->subscribe(Events::ENEMY_DIED, [this](const std::string& topic, void* data) {
        EnemyEventData* eventData = static_cast<EnemyEventData*>(data);
        if (eventData) {
            gold_ += eventData->goldReward;
            std::cout << "[Game] Enemy died! Gold: +" << eventData->goldReward 
                      << " (Total: " << gold_ << ")" << std::endl;
            
            if (eventData->enemy && eventData->enemy->transform) {
                showGoldText(eventData->goldReward, eventData->enemy->transform->position);
                particleSystem_->emitExplosion(eventData->enemy->transform->position, 20.0f);
            }
        }
    });
    
    // Enemy reached end - FIXED: Correct callback signature
    eventBus_->subscribe(Events::ENEMY_REACHED_END, [this](const std::string& topic, void* data) {
        lives_--;
        std::cout << "[Game] Enemy reached end! Lives remaining: " << lives_ << std::endl;
        
        if (lives_ <= 0) {
            std::cout << "[Game] ✗✗✗ GAME OVER ✗✗✗" << std::endl;
            gameStateManager_->defeat(); // FIXED: Correct method name
        }
        
        applyScreenShake(10.0f, 0.3f);
    });
    
    // Tower placed - FIXED: Correct callback signature
    eventBus_->subscribe(Events::TOWER_PLACED, [this](const std::string& topic, void* data) {
        TowerEventData* eventData = static_cast<TowerEventData*>(data);
        if (eventData) {
            std::cout << "[Game] Tower placed: " << eventData->towerType 
                      << " at (" << eventData->position.x << ", " << eventData->position.y << ")" << std::endl;
            particleSystem_->emit(eventData->position, Particle::SPARKLE, 15);
        }
    });
}

void Game::setupGameCallbacks() {
    std::cout << "[Game] Setting up game callbacks..." << std::endl;
}

void Game::setupEntityTextures() {
    std::cout << "[Game] Setting up entity textures..." << std::endl;
    
    // Setup tower textures
    for (const auto& tower : towerSystem_->getTowers()) {
        if (tower->sprite && tower->sprite->textureId != "MAIN_BASE") {
            std::string textureId = "tower_" + tower->towerType;
            // Remove "_tower" suffix if it exists
            size_t pos = textureId.find("_tower");
            if (pos != std::string::npos) {
                textureId.erase(pos, 6);
            }
            
            if (resourceManager_->hasTexture(textureId)) {
                tower->sprite->sprite.setTexture(resourceManager_->getTexture(textureId));
                tower->sprite->sprite.setOrigin(32, 32);
                tower->sprite->visible = true;
            }
        }
    }
    
    // Setup unit textures
    for (const auto& unit : unitSystem_->getUnits()) {
        if (unit->sprite) {
            std::string textureId = "unit_" + unit->unitType;
            
            if (resourceManager_->hasTexture(textureId)) {
                unit->sprite->sprite.setTexture(resourceManager_->getTexture(textureId));
                unit->sprite->sprite.setOrigin(16, 16);
                unit->sprite->visible = true;
            }
        }
    }
    
    // Setup enemy textures
    for (const auto& enemy : enemySystem_->getEnemies()) {
        if (enemy->sprite) {
            std::string textureId = "enemy_" + enemy->enemyType;
            
            if (resourceManager_->hasTexture(textureId)) {
                enemy->sprite->sprite.setTexture(resourceManager_->getTexture(textureId));
                enemy->sprite->sprite.setOrigin(16, 16);
                enemy->sprite->visible = true;
            }
        }
    }
    
    std::cout << "[Game] Entity textures setup complete" << std::endl;
}