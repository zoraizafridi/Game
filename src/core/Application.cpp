#include "Application.hpp"
#include "Game.hpp"
#include <iostream>

Application::Application(const std::string& title) : title_(title) {
    std::cout << "[Application] Constructor called" << std::endl;
}

Application::~Application() {
    std::cout << "[Application] Destructor called" << std::endl;
}

bool Application::initialize() {
    std::cout << "[Application] initialize() - Creating game..." << std::endl;
    game_ = std::make_unique<Game>();
    
    if (!game_) {
        std::cerr << "[Application] ERROR: Failed to create game\n";
        return false;
    }
    
    std::cout << "[Application] Game created, initializing..." << std::endl;
    if (!game_->initialize()) {
        std::cerr << "[Application] WARNING: Game initialization had issues\n";
        std::cerr << "[Application] Continuing anyway to see what works..." << std::endl;
        // Don't return false - let it try to run
    }
    
    std::cout << "[Application] Initialization complete!" << std::endl;
    return true;
}

int Application::run() {
    std::cout << "[Application] run() - Starting game loop..." << std::endl;
    
    if (!game_) {
        std::cerr << "[Application] ERROR: Game is null!" << std::endl;
        std::cout << "\nPress Enter to exit..." << std::endl;
        std::cin.get();
        return -1;
    }
    
    // Run the game loop
    int result = game_->run();
    
    // Keep console open
    std::cout << "\n=== Game Ended ===" << std::endl;
    std::cout << "Result code: " << result << std::endl;
    std::cout << "Press Enter to exit..." << std::endl;
    std::cin.get();
    
    return result;
}