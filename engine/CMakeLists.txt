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
        src/scripting/type-registration/VectorsToLua.cpp
        src/scripting/type-registration/InputToLua.cpp
        src/scripting/type-registration/EntityToLua.cpp
        src/scripting/type-registration/SceneToLua.cpp
        src/scripting/type-registration/ScriptingToLua.cpp
        src/scripting/type-registration/AssetManagerToLua.cpp
        src/scripting/type-registration/AssetLoaderToLua.cpp
        src/scripting/type-registration/ComponentsToLua.cpp
        src/scripting/type-registration/EngineToLua.cpp
        src/scripting/type-registration/PhysicsToLua.cpp
        src/scripting/type-registration/PhysicsBodyGetterToLua.cpp
        src/scripting/type-registration/PhysicsBodyUpdaterToLua.cpp
        src/scripting/type-registration/AiToLua.cpp
        src/scripting/type-registration/NavigationToLua.cpp
        src/scripting/type-registration/Vector2ToLua.cpp
        src/scripting/type-registration/Vector3ToLua.cpp
        src/scripting/type-registration/Vector4ToLua.cpp
        src/scripting/type-registration/QuatToLua.cpp
        src/scripting/type-registration/Mat4ToLua.cpp
        src/scripting/type-registration/DebugUIRendererToLua.cpp

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

        include/subsystems/PerceptionSubsystem.h
        src/subsystems/PerceptionSubsystem.cpp
        include/components/PerceptionComponent.h
        include/components/StimulusComponent.h
        include/ai/PerceptionEvents.h

        include/components/LightComponent.h

        include/components/CameraComponent.h
        src/Render/EditorCamera.cpp
        include/Render/EditorCamera.h
        src/Render/RuntimeCamera.cpp
        include/Render/RuntimeCamera.h
        include/Render/DebugRenderer.h
        src/Render/DebugRenderer.cpp
        include/components/SkyboxComponent.h
        include/components/NavMeshVolumeComponent.h
        include/subsystems/Navigation/NavigationSubsystem.h
        src/subsystems/Navigation/NavigationSubsystem.cpp
        include/subsystems/Navigation/NavmeshBuilder.h
        src/subsystems/Navigation/NavmeshBuilder.cpp
        include/Render/RenderTarget.h
        include/Render/GTexture.h
        include/Render/Resource.h
        src/Render/Resource.cpp
        src/Render/GTexture.cpp
        src/Render/RenderTarget.cpp
        include/tools/SelectionManager.h
        src/tools/SelectionManager.cpp
        src/scripting/type-registration/UUIDToLua.cpp
        include/Render/UI/UIRenderer.h
        include/Render/UI/DebugUIRenderer.h
        src/Render/UI/UIRenderer.cpp
        src/Render/UI/DebugUIRenderer.cpp
        include/tools/ComponentRegistry.h
        include/EngineConfig.h
        src/scene/SceneManager.cpp
        include/scene/SceneManager.h
        include/scene/SceneManagerTemplates.h
        include/tools/UUID.h
        include/components/PrefabComponent.h
        src/subsystems/PrefabSubsystem.cpp
        include/subsystems/PrefabSubsystem.h
        src/tools/Serializer.cpp
)

file(GLOB IMGUI_SOURCES
        "${IMGUI_DIR}/*.h"
        "${IMGUI_DIR}/*.cpp"
        "${IMGUI_DIR}/backends/imgui_impl_win32.h"
        "${IMGUI_DIR}/backends/imgui_impl_win32.cpp"
        "${IMGUI_DIR}/backends/imgui_impl_dx12.h"
        "${IMGUI_DIR}/backends/imgui_impl_dx12.cpp"
        "${IMGUIZMO_DIR}/*.h"
        "${IMGUIZMO_DIR}/*.cpp"
        "${CMAKE_SOURCE_DIR}/libs/sol2_ImGui_Bindings/*.h"
        "${CMAKE_SOURCE_DIR}/libs/sol2_ImGui_Bindings/*.cpp"
)

list(APPEND ENGINE_SOURCES ${IMGUI_SOURCES})

add_library(ENGINE STATIC ${ENGINE_SOURCES})

# Third-party sources from libs/ are compiled into ENGINE but should not be linted.
set_source_files_properties(${IMGUI_SOURCES} PROPERTIES SKIP_LINTING ON)

set_source_files_properties(
        src/Render/DDSTextureLoader.cpp
        include/Render/DDSTextureLoader.h
        PROPERTIES
        SKIP_LINTING ON
)

if(MSVC)
    file(GLOB IMGUIZMO_CPP_SOURCES "${IMGUIZMO_DIR}/*.cpp")
    if(IMGUIZMO_CPP_SOURCES)
        set_source_files_properties(
                ${IMGUIZMO_CPP_SOURCES}
                PROPERTIES
                COMPILE_OPTIONS "/WX-"
                SKIP_LINTING ON
        )
    endif()
    file(GLOB SOL_IMGUI_CPP_SOURCES "${CMAKE_SOURCE_DIR}/libs/sol2_ImGui_Bindings/*.cpp")
    if(SOL_IMGUI_CPP_SOURCES)
        set_source_files_properties(${SOL_IMGUI_CPP_SOURCES} PROPERTIES COMPILE_OPTIONS "/WX-")
    endif()
endif()


if(MSVC)
    target_compile_options(ENGINE PRIVATE /bigobj /W4 /WX)
else()
    target_compile_options(ENGINE PRIVATE -Wall -Wextra -Wpedantic -Werror)
endif()

target_compile_definitions(ENGINE PRIVATE NOMINMAX)

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
        "${CMAKE_SOURCE_DIR}/libs/recastnavigation/Recast/Include"
        "${CMAKE_SOURCE_DIR}/libs/recastnavigation/Detour/Include"
        "${IMGUI_DIR}"
        "${IMGUIZMO_DIR}"
        "${CMAKE_SOURCE_DIR}/libs/sol2_ImGui_Bindings"
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
        Recast
        Detour
        d3d12.lib
        d3dcompiler.lib)
