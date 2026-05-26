#pragma once

#include <string>
#include "json.hpp"

#include <raylib.h>

// Serialization for raylib's Vector2
inline void to_json(nlohmann::json& j, const Vector2& v) {
    j = nlohmann::json{{"x", v.x}, {"y", v.y}};
}

inline void from_json(const nlohmann::json& j, Vector2& v) {
    j.at("x").get_to(v.x);
    j.at("y").get_to(v.y);
}

class ProjectConf {
public:
    Vector2 res = {1280.0f, 720.0f};
    bool fullscreen = false;
    bool resize = false;
    std::string title = "Speed Type";

    NLOHMANN_DEFINE_TYPE_INTRUSIVE(ProjectConf, res, fullscreen, resize, title)
};
