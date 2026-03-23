#include "raylib.h"
#include "engine/SceneManager.hpp"
#include "HomeScene.hpp"

int main() {
    InitWindow(1280, 720, "SpeedType");
    SetTargetFPS(60);
    SetExitKey(KEY_NULL);

    SceneManager::Get().Push<HomeScene>();

    while (!WindowShouldClose() && !SceneManager::Get().GetQuitRequested()) {
        SceneManager::Get().Update(GetFrameTime());

        BeginDrawing();
        ClearBackground({ 18, 18, 18, 255 });
        SceneManager::Get().Draw();
        EndDrawing();
    }

    SceneManager::Get().Shutdown();
    CloseWindow();
    return 0;
}
