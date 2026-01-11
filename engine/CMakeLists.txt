cmake_minimum_required(VERSION 3.21...4.0.1)

project(ENGINE
        VERSION 0.0.1
        LANGUAGES CXX)

# write header first, then .cpp
# if you have only header, add in the end of the list
set(ENGINE_SOURCES
        include/Engine.h
        src/Engine.cpp
        include/subsystems/RenderSubsystem.h
        src/subsystems/RenderSubsystem.cpp
        include/subsystems/ScriptingSubsystem.h
        src/subsystems/ScriptingSubsystem.cpp
        include/subsystems/Log.h
        src/subsystems/Log.cpp
        include/components/MeshComponent.h
        src/components/MeshComponent.cpp
        include/Render/Camera.h
        include/Render/CascadeShadowMap.h
        include/Render/CommandQueue.h
        include/Render/DDSTextureLoader.h
        include/Render/Device.h
        include/Render/DXHelpers.h
        include/Render/FrameResource.h
        include/Render/FreyaCoreDefines.h
        include/Render/FreyaCoreTypes.h
        include/Render/FreyaMath.h
        include/Render/FreyaUtil.h
        include/Render/GBuffer.h
        include/Render/PipelineStateObject.h
        include/Render/PrebuiltEngineMeshes.h
        include/Render/RootSignature.h
        include/Render/SelectionManager.h
        include/Render/Shader.h
        include/Render/ShadowMap.h
        include/Render/SwapChain.h 
        include/Render/UploadBuffer.h
        src/Render/Camera.cpp
        src/Render/CascadeShadowMap.cpp
        src/Render/CommandQueue.cpp
        src/Render/DDSTextureLoader.cpp
        src/Render/Device.cpp
        src/Render/FrameResource.cpp
        src/Render/FreyaUtil.cpp
        src/Render/GBuffer.cpp
        src/Render/PipelineStateObject.cpp
        src/Render/PrebuiltEngineMeshes.cpp
        src/Render/RootSignature.cpp
        src/Render/SelectionManager.cpp
        src/Render/Shader.cpp
        src/Render/ShadowMap.cpp
        src/Render/SwapChain.cpp
        include/scene/Scene.h
        src/scene/Scene.cpp
        include/scene/Entity.h
        src/scene/Entity.cpp
        include/subsystems/AssetManager.h
        src/subsystems/AssetManager.cpp
        include/subsystems/AssetLoader.h
        src/subsystems/AssetLoader.cpp
        include/handles/Handle.h
        src/handles/Handle.cpp
        include/file-system/Texture.h
        src/file-system/Texture.cpp
        include/file-system/Material.h
        src/file-system/Material.cpp
        include/file-system/FileSystemObject.h
        src/file-system/FileSystemObject.cpp
        include/file-system/Model.h
        src/file-system/Model.cpp
        include/scripting/LuaScript.h
        src/scripting/LuaScript.cpp
        include/subsystems/input/InputSubsystem.h
        src/subsystems/InputSubsystem.cpp
        include/file-system/TextureType.h
        include/tools/Profiler.h
        include/tools/FreeListVector.h
        include/scene/BasicComponents.h
        include/scene/EntityTemplates.h
        include/components/ScriptingComponent.h
        include/tools/Timeline.h
        include/tools/PeriodicTimeline.h
        include/
        include/subsystems/input/InputEvent.h
        include/subsystems/input/KeyboardEvents.h
        include/subsystems/input/MouseEvents.h
        include/subsystems/input/KeyCodes.h

        include/scene/TransformComponent.h
        include/tools/Serializer.h
        include/scene/SceneEvent.h
        src/scene/SceneEvent.cpp
        
        include/scripting/TypeRegistration.h
        src/scripting/type-registration/CommonToLua.cpp
        src/scripting/type-registration/InputToLua.cpp
        src/scripting/type-registration/EntityToLua.cpp
        src/scripting/type-registration/SceneToLua.cpp
        src/scripting/type-registration/ScriptingToLua.cpp
        src/scripting/type-registration/AssetManagerToLua.cpp
        src/scripting/type-registration/AssetLoaderToLua.cpp
        src/scripting/type-registration/ComponentsToLua.cpp
        src/scripting/type-registration/EngineToLua.cpp
        src/scripting/type-registration/PhysicsToLua.cpp
        src/scripting/type-registration/AiToLua.cpp

        include/subsystems/PhysicsSubsystem.h
        src/subsystems/PhysicsSubsystem.cpp
        include/components/PhysicsComponent.h
        include/physics/PhysicsCreationSettings.h
        include/physics/RayCastResult.h
        include/physics/PhysicsEvents.h
        include/physics/ShapeFactory.h
        src/physics/ShapeFactory.cpp
        include/physics/BodyBuilder.h
        src/physics/BodyBuilder.cpp
        include/physics/BodyUpdater.h
        src/physics/BodyUpdater.cpp
        include/physics/BodyGetter.h
        src/physics/BodyGetter.cpp
        include/physics/ContactListenerImpl.h
        src/physics/ContactListenerImpl.cpp

        include/subsystems/AISubsystem.h
        src/subsystems/AISubsystem.cpp
        include/ai/AIController.h
        src/ai/AIController.cpp
        include/components/AIControllerComponent.h
        include/ai/Blackboard.h
        include/ai/BTNodes.h
        src/ai/BTNodes.cpp
        include/ai/BehaviourTree.h
        src/ai/BehaviourTree.cpp
        include/ai/BTBuilder.h
        src/ai/BTBuilder.cpp
        include/ai/UtilityBuilder.h
        src/ai/UtilityBuilder.cpp
        include/ai/UtilityContext.h
        src/ai/UtilityContext.cpp
        include/ai/UtilityDecision.h
        src/ai/UtilityDecision.cpp
        include/ai/UtilitySelector.h
        src/ai/UtilitySelector.cpp
        # src/ai/FromLuaToAIController.cpp

        include/components/CameraComponent.h
        src/Render/EditorCamera.cpp
        include/Render/EditorCamera.h
        src/Render/RuntimeCamera.cpp
        include/Render/RuntimeCamera.h
        include/Render/DebugRenderer.h
        src/Render/DebugRenderer.cpp

        include/components/SkyboxComponent.h
        include/Render/RenderTarget.h
        include/Render/Texture.h
        include/Render/Resource.h
        src/Render/Resource.cpp
        src/Render/Texture.cpp
        src/Render/RenderTarget.cpp
)

add_library(ENGINE STATIC ${ENGINE_SOURCES})

target_compile_options(ENGINE PRIVATE /bigobj)

source_group(TREE "${CMAKE_SOURCE_DIR}" FILES ${ENGINE_SOURCES})

target_include_directories(${PROJECT_NAME} PUBLIC
        "${PROJECT_SOURCE_DIR}/include"
        "${CMAKE_SOURCE_DIR}/libs/"
        "${JobSystem_DIR}/include"
        "${TracyClient_DIR}/public"
        "${spdlog_DIR}/include"
        "${CMAKE_SOURCE_DIR}/common"
        "${Entt_DIR}/single_include"
        "${CMAKE_SOURCE_DIR}/libs/concurrentqueue"
        "${LUA_INCLUDE_DIR}"
        "${SOL2_INCLUDE_DIR}"
        "${CMAKE_SOURCE_DIR}/libs/eventpp/include"
)


target_link_libraries(${PROJECT_NAME} PRIVATE
        EASTL
        DirectXTK12
        DirectXTex
        Jolt
        ozz_animation
        assimp::assimp
        TracyClient
        spdlog
        pch
        lua
        d3d12.lib
        d3dcompiler.lib)