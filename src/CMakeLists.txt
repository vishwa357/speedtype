file(GLOB GAME_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
set(PLATFORM "PLATFORM_DRM" CACHE STRING "Target platform")
add_executable(speedtype ${GAME_SOURCES})

target_include_directories(speedtype PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)
target_link_libraries(speedtype PRIVATE engine_lib)
