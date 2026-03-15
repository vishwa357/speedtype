cmake_minimum_required(VERSION 3.28)
project(typing_master CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(external/raylib)
add_subdirectory(external/entt)

file(GLOB SOURCES src/*.cpp engine/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/engine
    external/nlohmann
    external/raylib/src
)

target_link_libraries(${PROJECT_NAME} PRIVATE raylib EnTT::EnTT)
