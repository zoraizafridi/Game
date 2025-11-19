#include "../core/Application.hpp"
#include <iostream>
#include <exception>

int main() {
    std::cout << "=====================================" << std::endl;
    std::cout << "    TOWER DEFENSE GAME" << std::endl;
    std::cout << "=====================================" << std::endl;
    
    try {
        // Create application instance
        Application app("Tower Defense Game");
        
        // Initialize the application
        std::cout << "\n[Main] Initializing application..." << std::endl;
        if (!app.initialize()) {
            std::cerr << "[Main] Failed to initialize application." << std::endl;
            std::cout << "\nPress Enter to exit..." << std::endl;
            std::cin.get();
            return -1;
        }
        
        // Run the main game loop
        std::cout << "\n[Main] Starting game..." << std::endl;
        int result = app.run();
        
        std::cout << "\n[Main] Application exited with code: " << result << std::endl;
        return result;
        
    } catch (const std::exception& e) {
        std::cerr << "\n*** FATAL ERROR ***" << std::endl;
        std::cerr << "Exception: " << e.what() << std::endl;
        std::cout << "\nPress Enter to exit..." << std::endl;
        std::cin.get();
        return -1;
    } catch (...) {
        std::cerr << "\n*** FATAL ERROR ***" << std::endl;
        std::cerr << "Unknown exception caught!" << std::endl;
        std::cout << "\nPress Enter to exit..." << std::endl;
        std::cin.get();
        return -1;
    }
    return 0;
}