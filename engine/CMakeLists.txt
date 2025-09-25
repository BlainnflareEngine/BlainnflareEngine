cmake_minimum_required(VERSION 3.21...4.0.1)

project(ENGINE
        VERSION 0.0.1
        LANGUAGES CXX)

set(ENGINE_SOURCES
        src/Engine.cpp
        src/subsystems/Freya.cpp
        src/subsystems/Jord.cpp
        src/subsystems/Bifrost.cpp
        include/tools/Profiler.h
        include/subsystems/Log.h
        src/subsystems/Log.cpp
)

add_library(ENGINE STATIC ${ENGINE_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
        ${PROJECT_SOURCE_DIR}/include
        ${JobSystem_DIR}/include
        ${TracyClient_DIR}/public
        ${spdlog_DIR}/include
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

