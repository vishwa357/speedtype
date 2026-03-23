#include "SceneManager.hpp"

SceneManager& SceneManager::Get() {
    static SceneManager instance;
    return instance;
}

Scene* SceneManager::Current() {
    return current.get();
}

void SceneManager::ApplyPendingTransition() {
    if (!next) return;
    if (current) current->OnDestroy();
    current = std::move(next);
    next.reset();
    current->OnInit();
}

void SceneManager::Update(float dt) {
    ApplyPendingTransition();
    if (current) current->Update(dt);
}

void SceneManager::Draw() {
    if (current) current->Draw();
}

void SceneManager::Shutdown() {
    next.reset();
    if (current) {
        current->OnDestroy();
        current.reset();
    }
}

void SceneManager::RequestQuit() {
    quitRequested = true;
}
