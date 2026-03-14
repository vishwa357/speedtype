#pragma once
#include <memory>
#include <vector>
#include "Lesson.hpp"
#include "TypingEngine.hpp"
#include "Renderer.hpp"

enum class GameState {
    MainMenu,
    Playing,
    Results
};

class Game {
public:
    void Init();
    void Update(float dt);
    void Draw();
    void Shutdown();

private:
    GameState state = GameState::MainMenu;
    std::unique_ptr<Renderer> renderer;
    std::unique_ptr<TypingEngine> typingEngine;
    std::vector<LessonData> lessons;
    int currentLessonIndex = 0;
    int currentLineIndex   = 0;
    void LoadLessons(const std::string& path);
    void HandleInput();
};