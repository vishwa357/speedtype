#include "HomeScene.hpp"
#include "LessonScene.hpp"

static constexpr const char* FONT_PATHS[] = {
    "assets/fonts/DejaVuSansMono.ttf",
    "assets/fonts/JetBrainsMono-Regular.ttf",
    "assets/fonts/LiberationMono-Regular.ttf",
    "assets/fonts/font.ttf",
};

void HomeScene::OnInit() {
    sceneLoader.Load("scenes/home.json");
    LoadFont();
}

void HomeScene::OnDestroy() {
    Scene::OnDestroy();
    if (fontLoaded) {
        UnloadFont(font);
        fontLoaded = false;
    }
}

void HomeScene::OnDraw() {
    if (IsKeyPressed(KEY_ENTER)) {
        GoToLesson();
    }
    if (IsKeyPressed(KEY_ESCAPE)) {
        SceneManager::Get().RequestQuit();
    }
}

void HomeScene::LoadFont() {
    constexpr int SIZE = 32;
    for (const char* p : FONT_PATHS) {
        if (FileExists(p)) {
            font = LoadFontEx(p, SIZE, nullptr, 0);
            if (font.glyphCount > 0) {
                fontLoaded = true;
                return;
            }
            UnloadFont(font);
        }
    }
    font       = GetFontDefault();
    fontLoaded = false;
}

void HomeScene::GoToLesson() {
    SceneManager::Get().Push<LessonScene>(
        "scenes/lesson_scene.json",
        "data/content/lessons.json"
    );
}
