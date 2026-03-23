#include "Scene.hpp"

void Scene::OnDestroy() {
    for (auto& entity : entities) {
        entity->OnDestroy();
    }
    entities.clear();
    world.Clear();
    updateCallbacks.clear();
}

void Scene::Update(float dt) {
    for (auto& fn : updateCallbacks) {
        fn(dt);
    }
}

void Scene::Draw() {
    renderSystem.Draw();
    OnDraw();
}