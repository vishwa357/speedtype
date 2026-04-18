#pragma once
#include "World.hpp"
#include "raylib.h"
#include <iostream>

class RenderSystem {
public:
    explicit RenderSystem(World& world) : world(world) {}

    void Draw();
    void SetReferenceCanvasSize(float width, float height) {
        refCanvasWidth = width;
        refCanvasHeight = height;
    }

private:
    World& world;
    Font font;
    bool fontLoaded = false;

    // Reference canvas size for anchor-based positioning
    float refCanvasWidth = 1280.0f;
    float refCanvasHeight = 720.0f;

    void DrawSprites();
    void DrawLabels();
    void LoadSpriteTexture(Sprite& s);

    // Helper function to compute final position based on anchor and pivot
    Vector2 ComputeTransformedPosition(const Transform2D& t);
};
