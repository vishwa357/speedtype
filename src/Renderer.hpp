#pragma once
#include <string>
#include "raylib.h"
#include "TypingEngine.hpp"
#include "Lesson.hpp"

struct Theme {
    Color background;
    Color textUntyped;
    Color textCorrect;
    Color textWrong;
    Color cursor;
    Color tipText;
    Color uiAccent;
};

class Renderer {
public:
    void Init();
    void Shutdown();

    // per-frame draws
    void DrawLesson(
        const std::vector<CharResult>& results,
        int cursorPos,
        const std::string& tip
    );
    void DrawMainMenu(const std::vector<LessonData>& lessons, int selectedIndex);
    void DrawResults(float elapsedTime, int correctCount, int totalCount);
    void DrawHUD(float elapsedTime);

    void SetTheme(const Theme& theme);
    const Theme& GetTheme() const { return theme; }

    // helpers - public so Game can use for layout math if needed
    static int ScreenWidth()  { return GetScreenWidth(); }
    static int ScreenHeight() { return GetScreenHeight(); }

private:
    Theme theme;
    Font font;
    bool fontLoaded = false;

    void DrawTypingLine(
        const std::vector<CharResult>& results,
        int cursorPos,
        Vector2 origin,
        int fontSize
    );
    void DrawCursor(Vector2 pos, int fontSize);
};