#include "engine/PreviewTool.hpp"
#include <iostream>
#include <string>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: scene_previewer <scene_json_path>" << std::endl;
        std::cerr << "Example: scene_previewer scenes/lesson_scene.json" << std::endl;
        return 1;
    }

    std::string sceneJsonPath = argv[1];

    try {
        PreviewTool preview(sceneJsonPath);
        preview.Run();
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
