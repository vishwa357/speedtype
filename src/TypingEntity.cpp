#include "TypingEntity.hpp"
#include "../engine/Scene.hpp"
#include "raylib.h"
#include <cstdio>

void TypingEntity::OnInit() {
    typingEngine = std::make_unique<TypingEngine>();

    if (!lessons.empty()) {
        StartLine(0, 0);
    }

    // Register Tick as a Scene update callback. The lambda captures `this` —
    // safe because the entity is owned by the scene and destroyed before the
    // scene tears down its callback list.
    scene->RegisterUpdate([this](float dt) { Tick(dt); });
}

void TypingEntity::OnDestroy() {
    typingEngine.reset();
    // ECS components are owned by the World; World::Clear() in
    // Scene::OnDestroy() handles them. Nothing to do here.
}

void TypingEntity::Tick(float dt) {
    if (lessonComplete) return;

    HandleInput();
    typingEngine->Update(dt);

    if (typingEngine->GetState() == EngineState::LineComplete) {
        currentLineIndex++;

        const auto& lesson = lessons[static_cast<size_t>(currentLessonIndex)];
        if (currentLineIndex >= static_cast<int>(lesson.content.size())) {
            lessonComplete = true;
        } else {
            StartLine(currentLessonIndex, currentLineIndex);
        }
    }

    SyncLabels();
}

void TypingEntity::HandleInput() {
    // Drain the full char queue — raylib buffers up to 16 chars per frame,
    // useful on fast keyboards or when pasting short strings.
    int key = GetCharPressed();
    while (key > 0) {
        typingEngine->HandleInput(static_cast<char>(key));
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        typingEngine->Backspace();
    }
}

void TypingEntity::SyncLabels() {
    World& world = scene->GetWorld();

    // Tip label — show the hint for the current line, empty when done.
    if (tipEntity != entt::null && world.registry.valid(tipEntity)) {
        std::string tipStr;
        if (!lessonComplete &&
            !lessons.empty() &&
            static_cast<size_t>(currentLessonIndex) < lessons.size() &&
            static_cast<size_t>(currentLineIndex) <
                lessons[static_cast<size_t>(currentLessonIndex)].content.size())
        {
            tipStr = lessons[static_cast<size_t>(currentLessonIndex)]
                         .content[static_cast<size_t>(currentLineIndex)].tip;
        }
        world.Get<Label>(tipEntity).text = tipStr;
    }

    // HUD label — elapsed seconds, one decimal place.
    if (hudEntity != entt::null && world.registry.valid(hudEntity)) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.1fs", typingEngine->GetElapsedTime());
        world.Get<Label>(hudEntity).text = buf;
    }
}

void TypingEntity::StartLine(int lessonIdx, int lineIdx) {
    if (lessons.empty()) return;
    if (lessonIdx < 0 || static_cast<size_t>(lessonIdx) >= lessons.size()) return;

    const auto& lesson = lessons[static_cast<size_t>(lessonIdx)];
    if (lineIdx < 0 || static_cast<size_t>(lineIdx) >= lesson.content.size()) return;

    typingEngine->LoadLine(lesson.content[static_cast<size_t>(lineIdx)]);
    SyncLabels();
}

void TypingEntity::NextLesson() {
    if (static_cast<size_t>(currentLessonIndex + 1) >= lessons.size()) return;

    currentLessonIndex++;
    currentLineIndex = 0;
    lessonComplete = false;
    StartLine(currentLessonIndex, currentLineIndex);
}
