#pragma once
#include "../engine/Scene.hpp"
#include "../engine/SceneManager.hpp"
#include "../engine/SceneLoader.hpp"
#include "raylib.h"

// HomeScene — the first scene pushed by main.cpp.
//
// Responsibilities:
//   - Displays the game title and a "Press Enter to Start" prompt via ECS
//     Label entities (rendered by RenderSystem automatically)
//   - Listens for Enter key in OnDraw(); calls SceneManager::Get().Push<LessonScene>()
//     which defers the transition to the start of the next frame
//
// Layout is loaded from scenes/home.json so positions can be tweaked and
// hot-reloaded in DEBUG builds without recompiling.
//
// Font ownership:
//   HomeScene loads and owns its font. It is unloaded in OnDestroy() before
//   the scene is torn down. RenderSystem has its own font load path for the
//   Label pass — that is intentional: each scene is self-contained.

// Forward-declared here so the header compiles without pulling in LessonScene's
// full include chain. The Push<LessonScene>() call in the .cpp includes it.
class LessonScene;

class HomeScene : public Scene {
public:
    HomeScene() : Scene("HomeScene"), sceneLoader(world) {}

    void OnInit()    override;
    void OnDestroy() override;

protected:
    void OnDraw() override;

private:
    SceneLoader sceneLoader;

    Font font      = {};
    bool fontLoaded = false;

    void LoadFont();
    void GoToLesson();
};