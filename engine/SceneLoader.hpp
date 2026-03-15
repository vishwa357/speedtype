#pragma once
#include "World.hpp"
#include "json.hpp"
#include <fstream>
#include <string>
#include <unordered_map>

class SceneLoader {
public:
    explicit SceneLoader(World& world) : world(world) {}

    bool Load(const std::string& path);
    entt::entity Find(const std::string& id);

private:
    World& world;
    std::unordered_map<std::string, entt::entity> entityMap;

    void ParseComponents(entt::entity e, const nlohmann::json& c);
};