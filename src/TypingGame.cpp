#include "TypingGame.hpp"
#include "TypingEngine.hpp"
#include "Lesson.hpp"
#include "raylib.h"
#include "json.hpp"
#include <fstream>
#include <cstdio>

namespace {

constexpr int FONT_SIZE = 32;
const char* FONT_PATHS[] = {
    "assets/fonts/DejaVuSansMono.ttf",
    "assets/fonts/JetBrainsMono-Regular.ttf",
    "assets/fonts/font.ttf",
};

constexpr float TEXT_LINE_X = 100.0f;
constexpr float TEXT_LINE_Y = 300.0f;
constexpr float TIP_X = 100.0f;
constexpr float TIP_Y = 340.0f;
constexpr float HUD_X = 20.0f;
constexpr float HUD_Y = 20.0f;

} // namespace

TypingGame::TypingGame(World& world) : world(world) {}

void TypingGame::Init() {
    typingEngine = std::make_unique<TypingEngine>();
    LoadLessons("data/content/lessons.json");

    textLineEntity = world.Create();
    world.Add<Transform2D>(textLineEntity, Transform2D{ TEXT_LINE_X, TEXT_LINE_Y, 0.0f, 0.0f });
    world.Add<Label>(textLineEntity, Label{ "", FONT_SIZE, WHITE });
    world.Add<Visible>(textLineEntity, Visible{ false });  // drawn manually in Draw(), not by RenderSystem

    tipEntity = world.Create();
    world.Add<Transform2D>(tipEntity, Transform2D{ TIP_X, TIP_Y, 0.0f, 0.0f });
    world.Add<Label>(tipEntity, Label{ "", 20.0f, { 120, 120, 120, 255 } });
    world.Add<Visible>(tipEntity, Visible{ true });

    hudEntity = world.Create();
    world.Add<Transform2D>(hudEntity, Transform2D{ HUD_X, HUD_Y, 0.0f, 0.0f });
    world.Add<Label>(hudEntity, Label{ "0.0s", 20.0f, { 180, 180, 180, 255 } });
    world.Add<Visible>(hudEntity, Visible{ true });

    fontLoaded = false;
    for (const char* path : FONT_PATHS) {
        if (FileExists(path)) {
            font = LoadFontEx(path, FONT_SIZE, nullptr, 0);
            if (font.glyphCount > 0) {
                fontLoaded = true;
                break;
            }
            UnloadFont(font);
        }
    }
    if (!fontLoaded) {
        font = GetFontDefault();
    }

    if (!lessons.empty()) {
        currentLessonIndex = 0;
        currentLineIndex   = 0;
        state = GameState::Playing;
        StartLine(0, 0);
    }
}

void TypingGame::LoadLessons(const std::string& path) {
    std::ifstream f(path);
    if (!f) return;
    auto j = nlohmann::json::parse(f);
    lessons.clear();
    for (const auto& l : j["lessons"]) {
        LessonData ld;
        ld.id = l["id"].get<std::string>();
        ld.name = l["name"].get<std::string>();
        ld.description = l.contains("description") ? l["description"].get<std::string>() : "";
        for (const auto& c : l["content"]) {
            LessonLine line;
            line.text = c["text"].get<std::string>();
            line.tip = c.contains("tip") ? c["tip"].get<std::string>() : "";
            ld.content.push_back(line);
        }
        lessons.push_back(ld);
    }
}

void TypingGame::StartLine(int lessonIdx, int lineIdx) {
    if (lessons.empty() || lessonIdx < 0 || lineIdx < 0) return;
    if (static_cast<size_t>(lessonIdx) >= lessons.size()) return;
    auto& lesson = lessons[lessonIdx];
    if (static_cast<size_t>(lineIdx) >= lesson.content.size()) return;

    typingEngine->LoadLine(lesson.content[lineIdx]);
    SyncEntitiesToEngine();
}

void TypingGame::SyncEntitiesToEngine() {
    if (tipEntity != entt::null && world.registry.valid(tipEntity)) {
        std::string tipStr;
        if (state == GameState::Playing && !lessons.empty() &&
            static_cast<size_t>(currentLessonIndex) < lessons.size() &&
            static_cast<size_t>(currentLineIndex) < lessons[currentLessonIndex].content.size()) {
            tipStr = lessons[currentLessonIndex].content[currentLineIndex].tip;
        }
        world.Get<Label>(tipEntity).text = tipStr;
    }
    if (hudEntity != entt::null && world.registry.valid(hudEntity)) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.1fs", typingEngine->GetElapsedTime());
        world.Get<Label>(hudEntity).text = buf;
    }
}

void TypingGame::Draw() {
    if (state != GameState::Playing) return;

    const auto& results = typingEngine->GetResults();
    int cursorPos = typingEngine->GetCursorPos();

    float originX = TEXT_LINE_X;
    float originY = TEXT_LINE_Y;
    const float scale = static_cast<float>(FONT_SIZE) / static_cast<float>(font.baseSize);

    Color colorUntyped = { 180, 180, 180, 255 };
    Color colorCorrect = { 100, 220, 100, 255 };
    Color colorWrong   = { 220, 80, 80, 255 };
    Color colorCursor = { 255, 255, 255, 255 };

    Vector2 pos = { originX, originY };
    for (size_t i = 0; i < results.size(); ++i) {
        Color color;
        switch (results[i].state) {
            case CharState::Untyped: color = colorUntyped; break;
            case CharState::Correct: color = colorCorrect; break;
            case CharState::Wrong:   color = colorWrong;   break;
        }
        int codepoint = static_cast<unsigned char>(results[i].expected);
        DrawTextCodepoint(font, codepoint, pos, static_cast<float>(FONT_SIZE), color);
        GlyphInfo info = GetGlyphInfo(font, codepoint);
        float advance = (info.advanceX > 0)
            ? static_cast<float>(info.advanceX) * scale
            : static_cast<float>(info.image.width) * scale;
        pos.x += advance;
    }

    if (cursorPos >= 0 && cursorPos <= static_cast<int>(results.size())) {
        Vector2 cursorPosVec = { originX, originY };
        for (int i = 0; i < cursorPos && i < static_cast<int>(results.size()); ++i) {
            GlyphInfo info = GetGlyphInfo(font, static_cast<unsigned char>(results[i].expected));
            // cursorPosVec.x += static_cast<float>(info.advanceX) * scale;
            float advance = (info.advanceX > 0)
                ? static_cast<float>(info.advanceX) * scale
                : static_cast<float>(info.image.width) * scale;
            cursorPosVec.x += advance;
        }
        DrawRectangle(static_cast<int>(cursorPosVec.x), static_cast<int>(cursorPosVec.y), 2, FONT_SIZE, colorCursor);
    }
}

void TypingGame::Update(float dt) {
    HandleInput();

    if (state == GameState::Playing) {
        typingEngine->Update(dt);

        if (typingEngine->GetState() == EngineState::LineComplete) {
            currentLineIndex++;
            if (currentLineIndex >= static_cast<int>(lessons[currentLessonIndex].content.size())) {
                state = GameState::Results;
            } else {
                StartLine(currentLessonIndex, currentLineIndex);
            }
        }
    }

    SyncEntitiesToEngine();
}

void TypingGame::Shutdown() {
    typingEngine.reset();
}

void TypingGame::HandleInput() {
    if (state != GameState::Playing) return;

    int key = GetCharPressed();
    while (key > 0) {
        typingEngine->HandleInput(static_cast<char>(key));
        key = GetCharPressed();
    }
    if (IsKeyPressed(KEY_BACKSPACE)) {
        typingEngine->Backspace();
    }
}
