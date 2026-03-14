#include "raylib.h"
#include "Game.hpp"

int main() {
    InitWindow(1280, 720, "Typing Master");
    SetTargetFPS(60);

    Game game;
    game.Init();

    while (!WindowShouldClose()) {
        game.Update(GetFrameTime());

        BeginDrawing();
        ClearBackground(RAYWHITE);
        game.Draw();
        EndDrawing();
    }

    game.Shutdown();
    CloseWindow();
    return 0;
}
