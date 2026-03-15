#include "RenderSystem.hpp"

constexpr int FONT_SIZE = 32;
constexpr float SPACING = 0.0f;

const char* FONT_PATHS[] = {
    "assets/fonts/DejaVuSansMono.ttf",
    "assets/fonts/JetBrainsMono-Regular.ttf",
    "assets/fonts/LiberationMono-Regular.ttf",
    "assets/fonts/font.ttf",
};

void RenderSystem::Draw() {
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
    DrawSprites();
    DrawLabels();
}

void RenderSystem::DrawSprites() {
    auto view = world.registry.view<Transform2D, Sprite, Visible>();
    for (auto e : view) {
        auto& v = view.get<Visible>(e);
        if (!v.value) continue;

        auto& t = view.get<Transform2D>(e);
        auto& s = view.get<Sprite>(e);

        if (!s.loaded) LoadSpriteTexture(s);
        if (s.loaded)
            DrawTextureEx(s.texture, { t.x, t.y }, 0.0f, 1.0f, WHITE);
    }
}

void RenderSystem::DrawLabels() {
    auto view = world.registry.view<Transform2D, Label, Visible>();
    for (auto e : view) {
        auto& v = view.get<Visible>(e);
        if (!v.value) continue;

        auto& t = view.get<Transform2D>(e);
        auto& l = view.get<Label>(e);

        DrawTextEx(font,
            l.text.c_str(),
            {t.x, t.y},
            l.fontSize,
            SPACING,
            l.color);
    }
}

void RenderSystem::LoadSpriteTexture(Sprite& s) {
    if (FileExists(s.path.c_str())) {
        s.texture = LoadTexture(s.path.c_str());
        s.loaded  = (s.texture.id > 0);
    }
}
