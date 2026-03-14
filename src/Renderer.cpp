#include "Renderer.hpp"
#include <cstdio>

namespace {

constexpr int FONT_SIZE = 32;
constexpr float SPACING = 0.0f;

const char* FONT_PATHS[] = {
    "assets/fonts/DejaVuSansMono.ttf",
    "assets/fonts/JetBrainsMono-Regular.ttf",
    "assets/fonts/LiberationMono-Regular.ttf",
    "assets/fonts/font.ttf",
};

} // namespace

void Renderer::Init() {
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
}

void Renderer::Shutdown() {
    if (fontLoaded) {
        UnloadFont(font);
        fontLoaded = false;
    }
}

void Renderer::DrawTypingLine(
    const std::vector<CharResult>& results,
    int cursorPos,
    Vector2 origin,
    int fontSize
) {
    Vector2 pos = origin;
    const float scale = static_cast<float>(fontSize) / static_cast<float>(font.baseSize);

    for (size_t i = 0; i < results.size(); ++i) {
        Color color;
        switch (results[i].state) {
            case CharState::Untyped:  color = theme.textUntyped; break;
            case CharState::Correct:  color = theme.textCorrect; break;
            case CharState::Wrong:    color = theme.textWrong;   break;
        }

        int codepoint = static_cast<unsigned char>(results[i].expected);
        DrawTextCodepoint(font, codepoint, pos, static_cast<float>(fontSize), color);

        GlyphInfo info = GetGlyphInfo(font, codepoint);
        pos.x += static_cast<float>(info.advanceX) * scale;
    }

    if (cursorPos >= 0 && cursorPos <= static_cast<int>(results.size())) {
        Vector2 cursorOrigin = origin;
        for (int i = 0; i < cursorPos && i < static_cast<int>(results.size()); ++i) {
            GlyphInfo info = GetGlyphInfo(font, static_cast<unsigned char>(results[i].expected));
            cursorOrigin.x += static_cast<float>(info.advanceX) * scale;
        }
        DrawCursor(cursorOrigin, fontSize);
    }
}

void Renderer::DrawCursor(Vector2 pos, int fontSize) {
    DrawRectangle(static_cast<int>(pos.x), static_cast<int>(pos.y), 2, fontSize, theme.cursor);
}

void Renderer::DrawLesson(
    const std::vector<CharResult>& results,
    int cursorPos,
    const std::string& tip
) {
    int fontSize = FONT_SIZE;
    Vector2 origin = { static_cast<float>(ScreenWidth()) / 2.0f - 200.0f,
                      static_cast<float>(ScreenHeight()) / 2.0f - static_cast<float>(fontSize) / 2.0f };

    DrawTypingLine(results, cursorPos, origin, fontSize);

    if (!tip.empty()) {
        DrawTextEx(font, tip.c_str(), { origin.x, origin.y + static_cast<float>(fontSize) + 16.0f },
                  static_cast<float>(fontSize) * 0.6f, SPACING, theme.tipText);
    }
}

void Renderer::DrawMainMenu(const std::vector<LessonData>& lessons, int selectedIndex) {
    int y = 80;
    for (size_t i = 0; i < lessons.size(); ++i) {
        Color c = (static_cast<int>(i) == selectedIndex) ? theme.uiAccent : theme.textUntyped;
        DrawTextEx(font, lessons[i].name.c_str(), { 60.0f, static_cast<float>(y) },
                  static_cast<float>(FONT_SIZE), SPACING, c);
        y += FONT_SIZE + 12;
    }
}

void Renderer::DrawResults(float elapsedTime, int correctCount, int totalCount) {
    char buf[128];
    snprintf(buf, sizeof(buf), "Time: %.1fs  |  %d / %d correct", elapsedTime, correctCount, totalCount);
    int w = MeasureTextEx(font, buf, static_cast<float>(FONT_SIZE), SPACING).x;
    DrawTextEx(font, buf, { (ScreenWidth() - w) / 2.0f, ScreenHeight() / 2.0f - FONT_SIZE / 2.0f },
              static_cast<float>(FONT_SIZE), SPACING, theme.textUntyped);
}

void Renderer::DrawHUD(float elapsedTime) {
    char buf[32];
    snprintf(buf, sizeof(buf), "%.1fs", elapsedTime);
    DrawTextEx(font, buf, { 20.0f, 20.0f }, static_cast<float>(FONT_SIZE) * 0.7f, SPACING, theme.textUntyped);
}

void Renderer::SetTheme(const Theme& t) {
    theme = t;
}
