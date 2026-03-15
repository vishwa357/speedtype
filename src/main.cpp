#include "raylib.h"
#include "engine/World.hpp"
#include "engine/RenderSystem.hpp"
#include "engine/SceneLoader.hpp"
#include "engine/HotReload.hpp"
#include "TypingGame.hpp"

int main() {
    InitWindow(1280, 720, "Typing Master");
    SetTargetFPS(60);

    World       world;
    RenderSystem renderSystem(world);
    SceneLoader  sceneLoader(world);

    sceneLoader.Load("scenes/lesson_01.json");

#ifdef DEBUG
    HotReload hotReload("scenes/lesson_01.json", [&]() {
        sceneLoader.Load("scenes/lesson_01.json");
    });
#endif

    TypingGame game(world);
    game.Init();

    while (!WindowShouldClose()) {
#ifdef DEBUG
        hotReload.Poll();
#endif
        game.Update(GetFrameTime());

        BeginDrawing();
        ClearBackground({ 18, 18, 18, 255 });
        renderSystem.Draw();
        game.Draw();
        EndDrawing();
    }

    game.Shutdown();
    CloseWindow();
    return 0;
}
