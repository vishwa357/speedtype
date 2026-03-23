# README

**Project: SpeedType (formerly Typing Master)**
Linux now, Android next. C++17, Raylib, EnTT, nlohmann/json, CMake 3.28.

---

**DONE:**
- Raylib + EnTT + nlohmann wired into CMake
- Engine layer: `World`, `RenderSystem`, `SceneLoader`, `HotReload`
- Core ECS components: `Transform2D`, `Sprite`, `Label`, `Visible`
- `TypingEngine` — pure logic, char-by-char state (Untyped/Correct/Wrong)
- `Lesson` — loads from `data/content/lessons.json`
- `TypingGame` — coordinates engine + ECS, draws typing line manually
- Scene JSON hot-reload working in DEBUG
- Game runs: font, typing line, tip, timer all working

---

**CURRENT DISCUSSION — Architecture redesign:**

Moving from ad-hoc to a proper scene/entity system:

**Decisions made:**
- `SceneManager` — singleton, one scene at a time, handles transitions
- `SceneLoader` — separate from SceneManager, JSON → ECS entities, used by scenes in `OnInit()`
- `Entity` — base class with `OnInit()`, `OnDestroy()`. No virtual `Update()`
- `Scene` — special entity that has `Update()`. Other entities register update hooks with their scene explicitly
- Registration via C++ static macro (like `REGISTER_ENTITY`), not string-based JSON
- First scene hardcoded in `main.cpp` (`SceneManager::Get().Push<HomeScene>()`)
- `main.cpp` stays thin — init window, push first scene, run loop

**New engine headers designed (not yet implemented):**
- `engine/Entity.hpp` — base class, holds `entt::entity` handle + `Scene*`
- `engine/Scene.hpp` — owns `World`, `RenderSystem`, entity list, update callbacks
- `engine/SceneManager.hpp` — singleton, `Push<T>()`, `Update()`, `Draw()`, pending transition pattern

**What gets refactored:**
- `TypingGame` dissolves → becomes `LessonScene` (derives Scene) + `TypingEntity`
- `HotReload` stays, called inside scene's `OnInit()`
- `RenderSystem` moves inside `Scene`

---

**ENGINE LAYER (final shape):**
```
engine/
  Entity.hpp        ← base class
  Scene.hpp         ← base scene, owns world + render
  SceneManager.hpp  ← singleton, scene switching
  SceneLoader.hpp   ← JSON → ECS (unchanged)
  RenderSystem.hpp  ← inside Scene (unchanged)
  HotReload.hpp     ← unchanged
  World.hpp         ← unchanged
```

---

**GAME STRUCTURE (planned):**
```
games/typing_master/
  main.cpp                    ← thin, Push<HomeScene>
  HomeScene.hpp/cpp           ← title + menu buttons
  LessonSelectScene.hpp/cpp   ← lesson list, locked/unlocked
  LessonScene.hpp/cpp         ← replaces TypingGame
  CloudGameScene.hpp/cpp      ← cloud game
  entities/
    MenuButtonEntity.hpp      ← clickable button
    CloudEntity.hpp           ← pooled, has Velocity
    TypingEntity.hpp          ← wraps TypingEngine
  scenes/
    home.json
    lesson_select.json
    lesson_01.json
```

---

**PLANNED FEATURES:**
- Home screen: title + Lessons/Progress/Games buttons + bottom icons
- Lesson select: vertical list, completed/current/locked states
- Lesson screen: current typing game, refactored into new architecture
- Cloud game: falling word clouds, keyboard/hands overlay, object pooling via ECS
- No collision needed for any planned game
- Android NDK setup (next project after SpeedType)

---

**OPEN QUESTIONS (pick up here):**
- `REGISTER_ENTITY` macro design — static initializer pattern
- `HomeScene` and `LessonScene` implementation
- How `TypingEngine` + `Lesson` plug into new `LessonScene`
- Parallax background system
- Button hover/click handling (mouse input on entities)
