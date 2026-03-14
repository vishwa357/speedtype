#include "Game.hpp"
#include <fstream>
#include "json.hpp"

static Theme DefaultTheme() {
    return Theme {
        .background  = { 18,  18,  18,  255 },
        .textUntyped = { 180, 180, 180, 255 },
        .textCorrect = { 100, 220, 100, 255 },
        .textWrong   = { 220,  80,  80, 255 },
        .cursor      = { 255, 255, 255, 255 },
        .tipText     = { 120, 120, 120, 255 },
        .uiAccent    = { 100, 180, 255, 255 },
    };
}

void Game::Init() {
    renderer = std::make_unique<Renderer>();
    renderer->Init();
    renderer->SetTheme(DefaultTheme());

    typingEngine = std::make_unique<TypingEngine>();

    LoadLessons("data/content/lessons.json");

    if (!lessons.empty()) {
        currentLessonIndex = 0;
        currentLineIndex   = 0;
        typingEngine->LoadLine(lessons[currentLessonIndex].content[currentLineIndex]);
        state = GameState::Playing;
    }
}

void Game::Update(float dt) {
    HandleInput();
    if (state == GameState::Playing) {
        typingEngine->Update(dt);

        if (typingEngine->GetState() == EngineState::LineComplete) {
            currentLineIndex++;
            if (currentLineIndex >= (int)lessons[currentLessonIndex].content.size()) {
                state = GameState::Results;
            } else {
                typingEngine->LoadLine(
                    lessons[currentLessonIndex].content[currentLineIndex]
                );
            }
        }
    }
}

void Game::Draw() {
    ClearBackground({ 18, 18, 18, 255 });

    switch (state) {
        case GameState::MainMenu:
            renderer->DrawMainMenu(lessons, currentLessonIndex);
            break;
        case GameState::Playing: {
            const auto& line = lessons[currentLessonIndex].content[currentLineIndex];
            renderer->DrawLesson(
                typingEngine->GetResults(),
                typingEngine->GetCursorPos(),
                line.tip
            );
            renderer->DrawHUD(typingEngine->GetElapsedTime());
            break;
        }
        case GameState::Results:
            renderer->DrawResults(
                typingEngine->GetElapsedTime(),
                0, 0  // TODO: pass real counts from TypingEngine
            );
            break;
    }
}

void Game::Shutdown() {
    renderer->Shutdown();
}

void Game::LoadLessons(const std::string& path) {
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

void Game::HandleInput() {
    if (state == GameState::Playing) {
        int key = GetCharPressed();
        while (key > 0) {
            typingEngine->HandleInput(static_cast<char>(key));
            key = GetCharPressed();
        }
        if (IsKeyPressed(KEY_BACKSPACE))
            typingEngine->Backspace();
    }
}