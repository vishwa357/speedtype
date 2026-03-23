#pragma once
#include "World.hpp"
#include "raylib.h"
#include <iostream>

class RenderSystem {
public:
    explicit RenderSystem(World& world) : world(world) {}

    void Draw();

private:
    World& world;
    Font font;
    bool fontLoaded = false;

    void DrawSprites();
    void DrawLabels();
    void LoadSpriteTexture(Sprite& s);
};
