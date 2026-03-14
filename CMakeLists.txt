cmake_minimum_required(VERSION 3.20)
project(typing_master CXX)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(external/raylib)

file(GLOB_RECURSE SOURCES src/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    external/nlohmann
    external/raylib/src
)

target_link_libraries(${PROJECT_NAME} raylib)

