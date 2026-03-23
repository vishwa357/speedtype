#pragma once
#include "../engine/Scene.hpp"
#include "../engine/SceneLoader.hpp"
#include "../engine/SceneManager.hpp"
#include "../engine/HotReload.hpp"
#include "Lesson.hpp"
#include "TypingEntity.hpp"
#include "raylib.h"
#include "json.hpp"
#include <fstream>
#include <memory>
#include <optional>
#include <string>
#include <vector>

// LessonScene — a Scene that runs a single typing lesson.
//
// Startup order (OnInit):
//   1. LoadLessons()       — parse data/content/lessons.json into lesson data
//   2. LoadFont()          — try each candidate font path, fall back to default
//   3. sceneLoader.Load()  — parse scenes/lesson_01.json → ECS entities
//                            (text_line, tip, hud positions come from here)
//   4. CreateEntity<TypingEntity>()  — entity receives the three entt handles
//                                      that SceneLoader just produced; it owns
//                                      the TypingEngine and registers for update
//   5. HotReload watcher   — polls scenes/lesson_01.json; on change, re-runs
//                            sceneLoader.Load() and calls ReloadHandles() so
//                            TypingEntity keeps writing to valid ECS entities
//
// Per-frame (driven by SceneManager):
//   Update(dt)  → TypingEntity::Tick() via registered callback
//                 HotReload::Poll()    via registered callback
//   Draw()      → RenderSystem draws tip + hud Labels
//               → OnDraw() draws per-character coloured glyphs + cursor

class LessonScene : public Scene {
public:
    LessonScene(const std::string& sceneJsonPath,
                const std::string& lessonsJsonPath)
        : Scene("LessonScene")
        , sceneJsonPath(sceneJsonPath)
        , lessonsJsonPath(lessonsJsonPath)
        , sceneLoader(world)
    {}

    // -----------------------------------------------------------------------
    // Scene lifecycle
    // -----------------------------------------------------------------------

    void OnInit() override;

    void OnDestroy() override ;

protected:
    void OnDraw() override ;

private:
    std::string             sceneJsonPath;
    std::string             lessonsJsonPath;
    std::vector<LessonData> lessons;

    SceneLoader             sceneLoader;

    // Non-owning — actual ownership is with Scene::entities.
    TypingEntity*           typingEntity = nullptr;

    Font font      = {};
    bool fontLoaded = false;

#ifdef DEBUG
    std::optional<HotReload> hotReload;
#endif

    // -----------------------------------------------------------------------
    // Hot-reload handler
    // -----------------------------------------------------------------------

    void OnSceneReload();

    // -----------------------------------------------------------------------
    // Helpers
    // -----------------------------------------------------------------------

    void LoadLessons(const std::string& path);

    void LoadFont() ;

    float GlyphAdvance(int codepoint, float scale) const ;

    void DrawCompleteBanner() const ;
    void OnBack();
};
