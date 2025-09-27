cmake_minimum_required(VERSION 3.21...4.0.1)

project(ENGINE
        VERSION 0.0.1
        LANGUAGES CXX)

set(ENGINE_SOURCES
        src/Engine.cpp
        src/subsystems/RenderSubsystem.cpp
        src/subsystems/PhysicsSubsystem.cpp
        src/subsystems/ScriptingSubsystem.cpp
        src/subsystems/Log.cpp
        include/tools/Profiler.h
        include/subsystems/Log.h
)

add_library(ENGINE STATIC ${ENGINE_SOURCES})

source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${ENGINE_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${JobSystem_DIR}/include
        ${TracyClient_DIR}/public
        ${spdlog_DIR}/include
        ${Blain_common_dir}
)


target_link_libraries(${PROJECT_NAME} PRIVATE
        EASTL
        DirectXTK12
        Jolt
        ozz_animation
        assimp
        TracyClient
        spdlog
        pch
)

