cmake_minimum_required(VERSION 3.21...4.0.1)

project(ENGINE
        VERSION 0.0.1
        LANGUAGES CXX)

set(ENGINE_SOURCES
        src/subsystems/RenderSubsystem.cc
)

add_library(ENGINE STATIC ${ENGINE_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC ${PROJECT_SOURCE_DIR}/include ${JobSystem_DIR}/include)


target_link_libraries(${PROJECT_NAME} PRIVATE
        EASTL
        DirectXTK12
        Jolt
        ozz_animation
        assimp)

