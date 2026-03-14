## C++17 Raylib project. Typing Master for Linux, targeting Android later.
---
- Use nlohmann/json for data loading
- OOP: separate classes per system (Game, Lesson, TypingEngine, Renderer)
- Raylib for rendering, input, audio
- CMake build system
- Assets in assets/, data in data/
- No system tray, no global WPM overlay
- Prefer composition over inheritance
- Keep classes small and single-responsibility

## Directory struct
---
<proj-root>
├── CMakeLists.txt
├── .cursorrules
├── assets/
│   ├── images/
│   └── fonts/
├── data/
│   └── lessons.json
├── src/
│   ├── main.cpp
│   ├── Game.hpp / Game.cpp
│   ├── Lesson.hpp / Lesson.cpp
│   └── TypingEngine.hpp / TypingEngine.cpp
└── external/
    ├── raylib/
    └── nlohmann/
