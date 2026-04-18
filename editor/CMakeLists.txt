file(GLOB EDITOR_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(scene_previewer ${EDITOR_SOURCES})

target_include_directories(scene_previewer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(scene_previewer PRIVATE engine_lib)
