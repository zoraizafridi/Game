#pragma once
#include <memory>
#include <string>
#include <iostream>

class Game;

class Application {
public:
    Application(const std::string& title = "Tower Defense");
    ~Application();
    bool initialize();
    int run();
private:
    std::unique_ptr<Game> game_;
    std::string title_;
    int width_ = 1280;
    int height_ = 720;
};