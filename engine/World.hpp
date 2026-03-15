#pragma once
#include <entt/entt.hpp>
#include <string>
#include "raylib.h"

// Core components — extend as needed per game (Transform2D avoids raylib Transform)
struct Transform2D {
    float x = 0, y = 0;
    float w = 0, h = 0;
};

struct Sprite {
    std::string path;
    Texture2D   texture = {};
    bool        loaded  = false;
};

struct Label {
    std::string text;
    float       fontSize = 24.0f;
    Color       color    = WHITE;
};

struct Visible {
    bool value = true;
};

class World {
public:
    entt::registry registry;

    entt::entity Create()                  { return registry.create(); }
    void         Destroy(entt::entity e)   { registry.destroy(e); }

    template<typename T, typename... Args>
    T& Add(entt::entity e, Args&&... args) {
        return registry.emplace<T>(e, std::forward<Args>(args)...);
    }

    template<typename T>
    T& Get(entt::entity e)                 { return registry.get<T>(e); }

    template<typename T>
    bool Has(entt::entity e)               { return registry.all_of<T>(e); }

    template<typename T>
    void Remove(entt::entity e)            { registry.remove<T>(e); }

    void Clear()                           { registry.clear(); }
};