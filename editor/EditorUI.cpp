#include "EditorUI.hpp"
#include "rlImGui.h"
#include "imgui.h"

void MainMenuBar::Draw() {
    if (ImGui::BeginMainMenuBar()) {
        for (const auto& item : items) {
            item();
        }
        ImGui::EndMainMenuBar();
    }
}

void MainMenuBar::AddItem(std::function<void()> itemDrawFn) {
    items.push_back(std::move(itemDrawFn));
}

void EditorUI::Init() {
    rlImGuiSetup(true); // Initialize with dark theme
}

void EditorUI::Shutdown() {
    rlImGuiShutdown();
}

void EditorUI::BeginDraw() {
    rlImGuiBegin();
}

void EditorUI::DrawComponents() {
    for (const auto& component : components) {
        component->Draw();
    }
}

void EditorUI::EndDraw() {
    rlImGuiEnd();
}

void EditorUI::AddComponent(std::unique_ptr<UIComponent> component) {
    components.push_back(std::move(component));
}
