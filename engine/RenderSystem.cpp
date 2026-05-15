#include "RenderSystem.hpp"
#include "World.hpp"
#include <cstdio>
#include <iostream>

constexpr int FONT_SIZE = 32;
constexpr float SPACING = 0.0f;

const char* FONT_PATHS[] = {
    "assets/fonts/DejaVuSansMono.ttf",
    "assets/fonts/JetBrainsMono-Regular.ttf",
    "assets/fonts/LiberationMono-Regular.ttf",
    "assets/fonts/font.ttf",
};

Vector2 RenderSystem::ComputeTransformedPosition(const Transform2D& t) {
    // Calculate anchor position on canvas (normalized to pixel coordinates)
    float anchorX = refCanvasWidth * ((t.anchor.x0 + t.anchor.x1) * 0.5f);
    float anchorY = refCanvasHeight * ((t.anchor.y0 + t.anchor.y1) * 0.5f);

    // Calculate pivot offset (offset within the element based on size and pivot)
    float pivotOffsetX = t.w * t.pivot.x;
    float pivotOffsetY = t.h * t.pivot.y;

    // Final position: anchor point - pivot offset + x,y offset
    float finalX = anchorX - pivotOffsetX + t.x;
    float finalY = anchorY - pivotOffsetY + t.y;

    return { finalX, finalY };
}

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
        if (s.loaded) {
            Vector2 pos = ComputeTransformedPosition(t);
            DrawTextureEx(s.texture, pos, 0.0f, 1.0f, WHITE);
        }
    }
}

void RenderSystem::DrawLabels() {
    auto view = world.registry.view<Transform2D, Label, Visible>();
    for (auto e : view) {
        auto& v = view.get<Visible>(e);
        if (!v.value) continue;

        auto& t = view.get<Transform2D>(e);
        auto& l = view.get<Label>(e);

        Vector2 pos = ComputeTransformedPosition(t);
        DrawTextEx(font,
            l.text.c_str(),
            pos,
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
