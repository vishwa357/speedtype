#include "LessonScene.hpp"
#include "SceneManager.hpp"
#include "json.hpp"
#include "raylib.h"
#include "src/HomeScene.hpp"

void LessonScene::OnInit() {
    LoadLessons(lessonsJsonPath);
    LoadFont();

    // Populate the ECS registry from the scene JSON. This creates the
    // text_line, tip and hud entities at the positions defined in JSON —
    // the single source of truth for layout.
    sceneLoader.Load(sceneJsonPath);

    typingEntity = CreateEntity<TypingEntity>(
        std::vector<LessonData>(lessons),       // copy — entity owns its data
        sceneLoader.Find("text_line"),
        sceneLoader.Find("tip"),
        sceneLoader.Find("hud"),
        sceneLoader.Find("keyboard_img")
    );

#ifdef DEBUG
    // Watch the scene JSON for changes. When the file is saved, reload the
    // layout entities and hand the fresh handles back to the TypingEntity
    // so it never writes into stale registry slots.
    hotReload.emplace(sceneJsonPath, [this]() { OnSceneReload(); });

    // Poll every frame via the same update callback path everything else
    // uses — no special polling site needed in main.cpp.
    RegisterUpdate([this](float /*dt*/) { hotReload->Poll(); });
#endif
}

void LessonScene::OnDestroy() {
    // Let the base class notify entities and clear the registry first.
    Scene::OnDestroy();

    if (fontLoaded) {
        UnloadFont(font);
        fontLoaded = false;
    }
}

void LessonScene::OnDraw() {
    if (!typingEntity) return;
    if(IsKeyPressed(KEY_ESCAPE)) {
        OnBack();
        return;
    }

    if (typingEntity->IsLessonComplete()) {
        DrawCompleteBanner();
        return;
    }

    const auto& results   = typingEntity->GetResults();
    const int   cursorPos = typingEntity->GetCursorPos();

    // Read the authoritative origin from the ECS Transform2D so that
    // editing x/y in the scene JSON and hot-reloading immediately moves
    // the typing line on screen.
    entt::entity tlEntity = typingEntity->GetTextLineEntity();
    if (tlEntity == entt::null || !world.registry.valid(tlEntity)) return;

    auto& t = world.Get<Transform2D>(tlEntity);
    const float originX = t.x;
    const float originY = t.y;

    constexpr float DRAW_FONT_SIZE = 32.f;
    const float scale = fontLoaded
        ? DRAW_FONT_SIZE / static_cast<float>(font.baseSize)
        : 1.f;

    const Color colorUntyped = { 180, 180, 180, 255 };
    const Color colorCorrect = { 100, 220, 100, 255 };
    const Color colorWrong   = { 220,  80,  80, 255 };
    const Color colorCursor  = { 255, 255, 255, 255 };

    // --- per-character coloured glyphs ---
    Vector2 pos = { originX, originY };
    for (size_t i = 0; i < results.size(); ++i) {
        Color col;
        switch (results[i].state) {
            case CharState::Correct: col = colorCorrect; break;
            case CharState::Wrong:   col = colorWrong;   break;
            default:                 col = colorUntyped;  break;
        }
        const int cp = static_cast<unsigned char>(results[i].expected);
        DrawTextCodepoint(font, cp, pos, DRAW_FONT_SIZE, col);
        pos.x += GlyphAdvance(cp, scale);
    }

    // --- cursor bar ---
    if (cursorPos >= 0 && cursorPos <= static_cast<int>(results.size())) {
        Vector2 cursorVec = { originX, originY };
        for (int i = 0; i < cursorPos; ++i) {
            cursorVec.x += GlyphAdvance(
                static_cast<unsigned char>(results[static_cast<size_t>(i)].expected),
                scale);
        }
        DrawRectangle(
            static_cast<int>(cursorVec.x),
            static_cast<int>(cursorVec.y),
            2, static_cast<int>(DRAW_FONT_SIZE),
            colorCursor);
    }
}

void LessonScene::OnSceneReload() {
    // Re-parse the scene JSON. World::Clear() is called inside Load() so
    // all old component data is gone; fresh entities replace them.
    sceneLoader.Load(sceneJsonPath);

    if (typingEntity) {
        typingEntity->ReloadHandles(
            sceneLoader.Find("text_line"),
            sceneLoader.Find("tip"),
            sceneLoader.Find("hud"),
            sceneLoader.Find("keyboard_img"));
    }
}

void LessonScene::LoadLessons(const std::string& path) {
    std::ifstream f(path);
    if (!f) return;

    auto j = nlohmann::json::parse(f, nullptr, /*exceptions*/ false);
    if (j.is_discarded()) return;

    lessons.clear();
    for (const auto& l : j["lessons"]) {
        LessonData ld;
        ld.id          = l.value("id",          "");
        ld.name        = l.value("name",        "");
        ld.description = l.value("description", "");

        for (const auto& c : l["content"]) {
            LessonLine line;
            line.text = c.value("text", "");
            line.tip  = c.value("tip",  "");
            ld.content.push_back(std::move(line));
        }
        lessons.push_back(std::move(ld));
    }
}

void LessonScene::LoadFont() {
    constexpr int FONT_SIZE = 32;
    static constexpr const char* FONT_PATHS[] = {
        "assets/fonts/DejaVuSansMono.ttf",
        "assets/fonts/JetBrainsMono-Regular.ttf",
        "assets/fonts/LiberationMono-Regular.ttf",
        "assets/fonts/font.ttf",
    };

    for (const char* p : FONT_PATHS) {
        if (FileExists(p)) {
            font = LoadFontEx(p, FONT_SIZE, nullptr, 0);
            if (font.glyphCount > 0) {
                fontLoaded = true;
                return;
            }
            UnloadFont(font);
        }
    }
    // Raylib's built-in font — always valid, must not be passed to
    // UnloadFont, so fontLoaded stays false.
    font       = GetFontDefault();
    fontLoaded = false;
}

float LessonScene::GlyphAdvance(int codepoint, float scale) const {
    GlyphInfo info = GetGlyphInfo(font, codepoint);
    return (info.advanceX > 0)
        ? static_cast<float>(info.advanceX) * scale
        : static_cast<float>(info.image.width) * scale;
}

void LessonScene::DrawCompleteBanner() const {
    constexpr float FS      = 40.f;
    constexpr float SPACING = 1.f;
    const char*     msg     = "Lesson complete!";
    Vector2         sz      = MeasureTextEx(font, msg, FS, SPACING);
    DrawTextEx(font, msg,
        { (static_cast<float>(GetScreenWidth())  - sz.x) * 0.5f,
            (static_cast<float>(GetScreenHeight()) - sz.y) * 0.5f },
        FS, SPACING, Color{ 100, 220, 100, 255 });
}

void LessonScene::OnBack() {
    SceneManager::Get().Push<HomeScene>();
}
