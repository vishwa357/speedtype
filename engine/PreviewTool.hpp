#pragma once

#include "Scene.hpp"
#include "HotReload.hpp"
#include <map>
#include <memory>
#include <optional>
#include <string>

struct Resolution {
    int width;
    int height;
};

class PreviewTool {
public:
    explicit PreviewTool(const std::string& sceneJsonPath);
    ~PreviewTool();

    void Run();

private:
    std::string sceneJsonPath;
    std::unique_ptr<Scene> previewScene;
    std::map<std::string, Resolution> presets;
    std::string currentPreset;
    std::optional<HotReload> hotReload;
    bool shouldExit;

    void LoadPresets();
    void OnSceneReload();
    void HandleInput();
    void SwitchPreset(const std::string& presetName);
    void DrawOverlay() const;
};
