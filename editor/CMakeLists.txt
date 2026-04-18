add_executable(scene_previewer
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)
target_link_libraries(scene_previewer PRIVATE engine_lib)
