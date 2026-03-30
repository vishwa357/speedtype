#pragma once
#include "../engine/Entity.hpp"
#include "../engine/World.hpp"
#include "TypingEngine.hpp"
#include "Lesson.hpp"
#include "json.hpp"
#include <entt/entt.hpp>
#include <memory>
#include <vector>

// TypingEntity — drives a typing lesson inside a LessonScene.
//
// It does NOT create ECS entities. The three layout entities (text_line, tip,
// hud) are created by SceneLoader from scenes/lesson_scene.json and their handles
// are handed in via the constructor (and refreshed via ReloadHandles() after a
// hot-reload wipes and re-populates the registry).
//
// Responsibilities:
//   - Owns a TypingEngine (character-level state machine)
//   - Handles keyboard input each frame
//   - Advances through lines in the lesson
//   - Syncs engine state into the ECS Label components each frame so
//     RenderSystem always sees fresh tip + hud text
//   - Registers itself with Scene::RegisterUpdate() so it is driven by the
//     normal Scene::Update(dt) path — nothing needs to call Tick() explicitly
//
// Per-character coloured drawing is intentionally NOT done here. LessonScene
// owns the font and calls DrawTextCodepoint in OnDraw(), reading GetResults()
// and GetCursorPos() from this entity.

class TypingEntity : public Entity {
public:
    TypingEntity(entt::entity handle, Scene* scene,
                 std::vector<LessonData> lessons,
                 entt::entity textLineEntity,
                 entt::entity tipEntity,
                 entt::entity hudEntity,
                 entt::entity keyboardImgEntity)
        : Entity(handle, scene)
        , lessons(std::move(lessons))
        , textLineEntity(textLineEntity)
        , tipEntity(tipEntity)
        , hudEntity(hudEntity)
        , keyboardImgEntity(keyboardImgEntity)
    {}

    // -----------------------------------------------------------------------
    // Entity lifecycle
    // -----------------------------------------------------------------------

    void OnInit() override;
    void OnDestroy() override;

    // -----------------------------------------------------------------------
    // Hot-reload resync
    //
    // Called by LessonScene after SceneLoader re-parses the scene JSON and
    // rebuilds the registry. The old handles are now stale — this replaces
    // them with the freshly looked-up ones so SyncLabels() keeps writing to
    // valid entities.
    // -----------------------------------------------------------------------

    void ReloadHandles(entt::entity newTextLine,
                       entt::entity newTip,
                       entt::entity newHud,
                       entt::entity newKeyboardImg) {
        textLineEntity = newTextLine;
        tipEntity      = newTip;
        hudEntity      = newHud;
        keyboardImgEntity = newKeyboardImg;

        SyncLabels();
    }

    // -----------------------------------------------------------------------
    // Read-only accessors — used by LessonScene::OnDraw()
    // -----------------------------------------------------------------------

    const std::vector<CharResult>& GetResults()        const { return typingEngine->GetResults(); }
    int                            GetCursorPos()       const { return typingEngine->GetCursorPos(); }
    bool                           IsLessonComplete()   const { return lessonComplete; }
    entt::entity                   GetTextLineEntity()  const { return textLineEntity; }
    int                            GetCurrentLessonIndex() const { return currentLessonIndex; }
    float                          GetElapsedTime()     const { return typingEngine->GetElapsedTime(); }

    void NextLesson();

private:
    // -----------------------------------------------------------------------
    // Lesson state
    // -----------------------------------------------------------------------

    std::vector<LessonData> lessons;
    int  currentLessonIndex = 0;
    int  currentLineIndex   = 0;
    bool lessonComplete     = false;

    // -----------------------------------------------------------------------
    // ECS handles — owned by the World, not by this entity
    // -----------------------------------------------------------------------

    entt::entity textLineEntity = entt::null;
    entt::entity tipEntity      = entt::null;
    entt::entity hudEntity      = entt::null;
    entt::entity keyboardImgEntity = entt::null;

    // -----------------------------------------------------------------------
    // Engine
    // -----------------------------------------------------------------------

    std::unique_ptr<TypingEngine> typingEngine;

    // -----------------------------------------------------------------------
    // Private helpers
    // -----------------------------------------------------------------------

    void StartLine(int lessonIdx, int lineIdx);
    void HandleInput();
    void SyncLabels();

    // Invoked each frame via the lambda registered in OnInit().
    void Tick(float dt);
};
