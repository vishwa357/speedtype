#pragma once
#include "World.hpp"
#include "Entity.hpp"
#include "RenderSystem.hpp"
#include <functional>
#include <memory>
#include <string>
#include <vector>

using UpdateFn = std::function<void(float)>;

// Scene — owns a World, a RenderSystem, and all Entities that belong to it.
//
// Derived scenes override:
//   OnInit()    — create entities, load data, register update callbacks
//   OnDestroy() — release non-ECS resources (fonts, audio, etc.)
//   OnDraw()    — custom drawing that isn't expressed as ECS components
//
// Everything else (Update, Draw, entity creation, callback registration)
// is handled by the base class.
//
// Frame order enforced by SceneManager:
//   1. ApplyPendingTransition()  — swap in next scene at frame boundary
//   2. current->Update(dt)       — fires all registered UpdateFn callbacks
//   3. current->Draw()           — RenderSystem pass, then OnDraw()
class Scene {
public:
    explicit Scene(const std::string& name) : name(name) {}
    virtual ~Scene() = default;

    // Lifecycle — override in derived scenes.
    // OnInit and OnDestroy are NOT called by the base implementations;
    // SceneManager calls them at the right moment.
    virtual void OnInit()    {}
    virtual void OnDestroy();

    // Per-frame — NOT virtual. SceneManager calls these directly.
    void Update(float dt);
    void Draw();

    // Safe to call from within CreateEntity<T>() since OnInit() runs before
    // the entity pointer is returned.
    void RegisterUpdate(UpdateFn fn) {
        updateCallbacks.push_back(std::move(fn));
    }

    // Entity factory — scene takes ownership via unique_ptr.
    // T must derive from Entity. Its constructor must accept
    // (entt::entity, Scene*, Args...) as the first two arguments.
    // OnInit() is called on the new entity before the pointer is returned.
    template<typename T, typename... Args>
    T* CreateEntity(Args&&... args) {
        entt::entity handle = world.Create();
        auto entity = std::make_unique<T>(handle, this,
            std::forward<Args>(args)...);
        T* ptr = entity.get();
        entities.push_back(std::move(entity));
        ptr->OnInit();
        return ptr;
    }

    World&             GetWorld()  { return world; }
    const std::string& GetName()   const { return name; }

protected:
    // Override for drawing that can't be expressed as ECS components —
    // e.g. per-character coloured glyphs in LessonScene.
    // Called by Draw() after RenderSystem::Draw() completes.
    virtual void OnDraw() {}

    // Accessible to derived scenes (e.g. LessonScene reads registry directly).
    World        world;
    RenderSystem renderSystem{ world };
    std::string  name;

private:
    std::vector<std::unique_ptr<Entity>> entities;
    std::vector<UpdateFn>                updateCallbacks;
};
