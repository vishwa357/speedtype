#include "PreviewTool.hpp"
#include "SceneLoader.hpp"
#include "raylib.h"
#include <iostream>

PreviewTool::PreviewTool(const std::string& sceneJsonPath)
    : sceneJsonPath(sceneJsonPath), currentPreset("Desktop"), shouldExit(false) {
    previewScene = std::make_unique<Scene>("Preview");
    LoadPresets();
    std::cout << "creating preview" << std::endl;

    // Load the scene
    SceneLoader loader(previewScene->GetWorld());
    loader.Load(sceneJsonPath);

    // Set initial reference canvas size
    if (presets.count(currentPreset)) {
        const auto& res = presets[currentPreset];
        previewScene->GetRenderSystem().SetReferenceCanvasSize(
            static_cast<float>(res.width),
            static_cast<float>(res.height)
        );
    }

    // Set up hot-reload for the scene file
    hotReload.emplace(sceneJsonPath, [this]() { OnSceneReload(); });

    // Call OnInit on the preview scene
    previewScene->OnInit();
}

PreviewTool::~PreviewTool() {
    if (previewScene) {
        previewScene->OnDestroy();
    }
}

void PreviewTool::LoadPresets() {
    presets["Desktop"] = {1280, 720};
    presets["Mobile"] = {720, 1280};
    presets["Tablet"] = {1200, 1600};
    presets["Free Size"] = {GetScreenWidth(), GetScreenHeight()};
}

void PreviewTool::OnSceneReload() {
    std::cout << "Scene changed, reloading..." << "\n";
    previewScene->OnDestroy();
    previewScene->GetWorld().Clear();

    SceneLoader loader(previewScene->GetWorld());
    loader.Load(sceneJsonPath);
    previewScene->OnInit();
}

void PreviewTool::SwitchPreset(const std::string& presetName) {
    if (!presets.count(presetName)) {
        std::cout << "Preset not found: " << presetName << "\n";
        return;
    }

    currentPreset = presetName;
    const auto& res = presets[presetName];

    if (presetName != "Free Size") {
        SetWindowMinSize(320, 240);
        SetWindowMaxSize(0, 0);  // No max
        SetWindowSize(res.width, res.height);
    }

    previewScene->GetRenderSystem().SetReferenceCanvasSize(
        static_cast<float>(res.width),
        static_cast<float>(res.height)
    );
}

void PreviewTool::HandleInput() {
    // Cycle presets with P
    if (IsKeyPressed(KEY_P)) {
        static int presetIndex = 0;
        auto it = presets.begin();
        std::advance(it, (presetIndex++) % presets.size());
        SwitchPreset(it->first);
    }

    // Manual reload with R
    if (IsKeyPressed(KEY_R)) {
        OnSceneReload();
    }

    // Exit with ESC
    if (IsKeyPressed(KEY_ESCAPE)) {
        shouldExit = true;
    }

    // Handle window resize (switch to Free Size)
    if (IsWindowResized() && currentPreset != "Free Size") {
        currentPreset = "Free Size";
        presets["Free Size"] = {GetScreenWidth(), GetScreenHeight()};
        previewScene->GetRenderSystem().SetReferenceCanvasSize(
            static_cast<float>(GetScreenWidth()),
            static_cast<float>(GetScreenHeight())
        );
    }
}

void PreviewTool::DrawOverlay() const {
    const int fontSize = 16;
    const int lineHeight = fontSize + 4;
    const int padding = 10;

    // Draw semi-transparent background for text
    DrawRectangle(0, 0, 400, 6 * lineHeight + 2 * padding, {0, 0, 0, 180});

    // Draw text info
    DrawTextEx(GetFontDefault(), "Preview Tool", {padding, padding}, fontSize, 1.0f, WHITE);

    std::string fileText = "File: " + sceneJsonPath;
    DrawTextEx(GetFontDefault(), fileText.c_str(), {padding, padding + lineHeight}, fontSize, 1.0f, LIGHTGRAY);

    std::string presetText = "Preset: " + currentPreset;
    DrawTextEx(GetFontDefault(), presetText.c_str(), {padding, padding + 2 * lineHeight}, fontSize, 1.0f, LIGHTGRAY);

    std::string sizeText = "Size: " + std::to_string(GetScreenWidth()) + "x" + std::to_string(GetScreenHeight());
    DrawTextEx(GetFontDefault(), sizeText.c_str(), {padding, padding + 3 * lineHeight}, fontSize, 1.0f, LIGHTGRAY);

    const char* helpText = "P: Cycle Presets | R: Reload | ESC: Exit";
    DrawTextEx(GetFontDefault(), helpText, {padding, padding + 5 * lineHeight}, fontSize, 1.0f, YELLOW);
}

void PreviewTool::Run() {
    // Initialize window
    const auto& initialRes = presets[currentPreset];
    InitWindow(initialRes.width, initialRes.height, "SpeedType - Scene Preview");
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    SetTargetFPS(60);

    while (!WindowShouldClose() && !shouldExit) {
        HandleInput();
        if (hotReload) hotReload->Poll();

        previewScene->Update(GetFrameTime());

        BeginDrawing();
        ClearBackground({18, 18, 18, 255});
        previewScene->Draw();
        DrawOverlay();
        EndDrawing();
    }

    CloseWindow();
}
