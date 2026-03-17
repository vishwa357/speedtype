#include "SceneLoader.hpp"

bool SceneLoader::Load(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return false;

    nlohmann::json root = nlohmann::json::parse(file, nullptr, false);
    if (root.is_discarded()) return false;

    world.Clear();
    entityMap.clear();

    for (auto& ejson : root["entities"]) {
        auto entity = world.Create();
        std::string id = ejson.value("id", "");
        if (!id.empty()) entityMap[id] = entity;

        ParseComponents(entity, ejson["components"]);
    }
    return true;
}

entt::entity SceneLoader::Find(const std::string& id) {
    auto it = entityMap.find(id);
    return (it != entityMap.end()) ? it->second : entt::null;
}

void SceneLoader::ParseComponents(entt::entity e, const nlohmann::json& c) {
if (c.contains("Transform2D")) {
            auto& j = c["Transform2D"];
            world.Add<Transform2D>(e, Transform2D{
            j.value("x", 0.0f), j.value("y", 0.0f),
            j.value("w", 0.0f), j.value("h", 0.0f)
        });
    }
    if (c.contains("Sprite")) {
        world.Add<Sprite>(e, Sprite{
            c["Sprite"].value("path", "")
        });
    }
    if (c.contains("Label")) {
        auto& j = c["Label"];
        auto col = j.value("color", nlohmann::json::array({255,255,255,255}));
        world.Add<Label>(e, Label{
            j.value("text", ""),
            j.value("fontSize", 24.0f),
            Color{ col[0], col[1], col[2], col[3] }
        });
    }
    if (c.contains("Visible")) {
        world.Add<Visible>(e, Visible{
            c["Visible"].value("value", true)
        });
    }
}
