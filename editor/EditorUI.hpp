#pragma once

#include <vector>
#include <memory>
#include <functional>
#include <string>

// Base class for any UI component in the editor
class UIComponent {
public:
    virtual ~UIComponent() = default;
    
    // Draw the UI component using ImGui
    virtual void Draw() = 0;
};

// A full-width horizontal menu bar at the top of the window
class MainMenuBar : public UIComponent {
public:
    void Draw() override;
    
    // Add an item to be drawn inside the menu bar
    void AddItem(std::function<void()> itemDrawFn);

private:
    std::vector<std::function<void()>> items;
};

// Manages the ImGui lifecycle and renders all registered components
class EditorUI {
public:
    void Init();
    void Shutdown();
    
    void BeginDraw();
    void DrawComponents();
    void EndDraw();

    void AddComponent(std::unique_ptr<UIComponent> component);

private:
    std::vector<std::unique_ptr<UIComponent>> components;
};
