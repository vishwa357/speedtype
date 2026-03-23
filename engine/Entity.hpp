#pragma once
#include <entt/entt.hpp>

class Scene;

class Entity {
public:
    explicit Entity(entt::entity handle, Scene* scene)
        : handle(handle), scene(scene) {}

    virtual ~Entity() = default;

    virtual void OnInit()    {}
    virtual void OnDestroy() {}

    entt::entity GetHandle() const { return handle; }
    Scene*       GetScene()  const { return scene; }

protected:
    entt::entity handle = entt::null;
    Scene*       scene  = nullptr;
};
