#pragma once
#include "Scene.hpp"
#include <memory>

// SceneManager — singleton, owns the active scene and one pending next scene.
//
// Transition contract:
//   Push<T>() never switches immediately. It constructs T and stores it in
//   `next`. ApplyPendingTransition() is called at the top of Update(), so the
//   swap always happens at the start of a fresh frame — never mid-frame while
//   update callbacks or draw calls are still in flight.
//
// Each scene owns its own RenderSystem. When a transition fires, the outgoing
// scene's OnDestroy() tears down its World + RenderSystem fully before the
// incoming scene's OnInit() runs. No shared renderer state between scenes.

class SceneManager {
public:
    static SceneManager& Get();

    // Queue a scene transition. T must derive from Scene.
    // The new scene is constructed immediately but not made active until the
    // start of the next frame (inside Update → ApplyPendingTransition).
    template<typename T, typename... Args>
    void Push(Args&&... args) {
        next = std::make_unique<T>(std::forward<Args>(args)...);
    }

    // Call once per frame from main loop — applies any pending transition
    // first, then drives the active scene's update callbacks.
    void Update(float dt);

    // Call once per frame after Update — draws the active scene.
    void Draw();

    // Call on application exit — tears down the active scene gracefully so
    // entities can release GPU/audio resources before the window closes.
    void Shutdown();

    // Non-owning pointer to the current scene. Null before the first Push.
    Scene* Current();
    void RequestQuit();
    bool GetQuitRequested() const { return quitRequested; }

private:
    SceneManager() = default;

    // Performs the deferred scene swap: tears down current, moves next into
    // current, calls OnInit() on the incoming scene.
    void ApplyPendingTransition();

    std::unique_ptr<Scene> current;
    std::unique_ptr<Scene> next;
    bool quitRequested = false;
};
