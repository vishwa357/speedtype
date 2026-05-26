file(GLOB EDITOR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

set(IMGUI_DIR ${CMAKE_SOURCE_DIR}/external/imgui)
set(RLIMGUI_DIR ${CMAKE_SOURCE_DIR}/external/rlImGui)

set(IMGUI_SOURCES
    ${IMGUI_DIR}/imgui.cpp
    ${IMGUI_DIR}/imgui_draw.cpp
    ${IMGUI_DIR}/imgui_tables.cpp
    ${IMGUI_DIR}/imgui_widgets.cpp
    ${RLIMGUI_DIR}/rlImGui.cpp
)

add_executable(scene_previewer ${EDITOR_SOURCES} ${IMGUI_SOURCES})

target_include_directories(scene_previewer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${IMGUI_DIR}
    ${RLIMGUI_DIR}
)
target_link_libraries(scene_previewer PRIVATE engine_lib)
