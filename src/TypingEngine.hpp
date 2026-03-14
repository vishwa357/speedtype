#pragma once
#include <string>
#include <vector>
#include "Lesson.hpp"

enum class CharState {
    Untyped,
    Correct,
    Wrong
};

struct CharResult {
    char expected;
    CharState state;
};

enum class EngineState {
    Idle,
    Typing,
    LineComplete,
    LessonComplete
};

class TypingEngine {
public:
    void LoadLine(const LessonLine& line);
    void HandleInput(char c);
    void Backspace();
    void Update(float dt);
    void Reset();

    // state queries
    EngineState GetState() const { return state; }
    const std::vector<CharResult>& GetResults() const { return results; }
    int GetCursorPos() const { return cursorPos; }
    float GetElapsedTime() const { return elapsedTime; }
    bool IsComplete() const { return state == EngineState::LessonComplete; }

private:
    std::vector<CharResult> results;
    EngineState state = EngineState::Idle;
    int cursorPos = 0;
    float elapsedTime = 0.0f;

    bool IsLineComplete() const;
};