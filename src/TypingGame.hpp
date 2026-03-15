#pragma once
#include "../../engine/World.hpp"
#include "Lesson.hpp"
#include "TypingEngine.hpp"
#include <memory>
#include <vector>

enum class GameState {
    MainMenu,
    Playing,
    Results
};

// Typing-specific components — not in engine, game-local
struct TypingLine {
    int lessonIndex = 0;
    int lineIndex   = 0;
};

class TypingGame {
public:
    explicit TypingGame(World& world);

    void Init();
    void Update(float dt);
    void Draw();
    void Shutdown();

private:
    World&        world;
    GameState     state = GameState::MainMenu;
    int           currentLessonIndex = 0;
    int           currentLineIndex   = 0;
    Font font;
    bool fontLoaded = false;

    std::vector<LessonData>          lessons;
    std::unique_ptr<TypingEngine>    typingEngine;

    // EnTT entities owned by this game
    entt::entity  textLineEntity = entt::null;
    entt::entity  tipEntity      = entt::null;
    entt::entity  hudEntity      = entt::null;

    void LoadLessons(const std::string& path);
    void StartLine(int lessonIdx, int lineIdx);
    void HandleInput();
    void SyncEntitiesToEngine();  // pushes engine state into ECS labels
};
