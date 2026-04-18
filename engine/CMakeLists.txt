file(GLOB ENGINE_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_library(engine_lib STATIC ${ENGINE_SOURCES})

target_compile_definitions(engine_lib PUBLIC
    $<$<CONFIG:Debug>:DEBUG>
)
target_include_directories(engine_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/external/nlohmann
    ${CMAKE_SOURCE_DIR}/external/raylib/src
)
target_link_libraries(engine_lib PUBLIC raylib EnTT::EnTT)
