#include "TypingEngine.hpp"

void TypingEngine::LoadLine(const LessonLine& line) {
    results.clear();
    results.reserve(line.text.size());
    for (char ch : line.text) {
        results.push_back(CharResult{ch, CharState::Untyped});
    }
    cursorPos = 0;
    elapsedTime = 0.0f;
    state = results.empty() ? EngineState::LineComplete : EngineState::Typing;
}

void TypingEngine::HandleInput(char c) {
    if (state != EngineState::Typing) return;
    if (cursorPos < 0 || cursorPos >= static_cast<int>(results.size())) return;

    CharResult& current = results[cursorPos];
    current.state = (c == current.expected) ? CharState::Correct : CharState::Wrong;
    cursorPos++;

    if (IsLineComplete()) {
        state = EngineState::LineComplete;
    }
}

void TypingEngine::Backspace() {
    if (state == EngineState::Idle) return;
    if (cursorPos <= 0) return;

    cursorPos--;
    if (cursorPos >= 0 && cursorPos < static_cast<int>(results.size())) {
        results[cursorPos].state = CharState::Untyped;
    }
    if (!IsLineComplete()) {
        state = EngineState::Typing;
    }
}

void TypingEngine::Update(float dt) {
    if (state == EngineState::Idle) return;
    elapsedTime += dt;
}

void TypingEngine::Reset() {
    results.clear();
    cursorPos = 0;
    elapsedTime = 0.0f;
    state = EngineState::Idle;
}

bool TypingEngine::IsLineComplete() const {
    if (results.empty()) return false;
    for (const auto& r : results) {
        if (r.state == CharState::Untyped) return false;
    }
    return true;
}

