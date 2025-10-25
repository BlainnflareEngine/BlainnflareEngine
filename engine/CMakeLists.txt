cmake_minimum_required(VERSION 3.21...4.0.1)

project(ENGINE
        VERSION 0.0.1
        LANGUAGES CXX)

set(ENGINE_SOURCES
        src/Engine.cpp
        src/subsystems/RenderSubsystem.cpp
        src/subsystems/PhysicsSubsystem.cpp
        include/subsystems/ScriptingSubsystem.h
        src/subsystems/ScriptingSubsystem.cpp
        src/subsystems/Log.cpp
        src/runtime/physics/BodyBuilder.cpp
        src/runtime/physics/ContactListenerImpl.cpp
        src/runtime/components/PhysicsComponent.cpp
        src/runtime/physics/ShapeFactory.cpp
        include/scene/Scene.h
        src/scene/Scene.cpp
        include/scene/Entity.h
        src/scene/Entity.cpp
        include/scene/BasicComponents.h
        include/scene/EntityTemplates.h
        src/subsystems/AssetManager.cpp
        src/subsystems/AssetLoader.cpp
        include/subsystems/AssetLoader.h
        include/file-system/Material.h
        include/handles/Handle.h
        src/file-system/Texture.cpp
        src/file-system/Material.cpp
        src/handles/Handle.cpp
        src/file-system/FileSystemObject.cpp
        src/file-system/Model.cpp
        include/file-system/Model.h
        include/file-system/TextureType.h
        include/tools/FreeListVector.h
        include/runtime/scripting/LuaScript.h
        src/runtime/scripting/LuaScript.cpp
        include/runtime/components/ScriptingComponent.h
)

add_library(ENGINE STATIC ${ENGINE_SOURCES})

source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${ENGINE_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/libs/"
        "${JobSystem_DIR}/include"
        "${TracyClient_DIR}/public"
        "${spdlog_DIR}/include"
        "${Blain_common_dir}"
        "${CMAKE_SOURCE_DIR}/common"
        "${Entt_DIR}/single_include"
        "${CMAKE_SOURCE_DIR}/libs/concurrentqueue"
        "${LUA_INCLUDE_DIR}"
        "${SOL2_INCLUDE_DIR}"
)


target_link_libraries(${PROJECT_NAME} PRIVATE
        EASTL
        DirectXTK12
        Jolt
        ozz_animation
        assimp::assimp
        TracyClient
        spdlog
        pch
        lua
)

