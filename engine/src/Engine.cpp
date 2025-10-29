#include "pch.h"

#include "Engine.h"

#include "VGJS.h"

#include "aliases.h"
#include "scene/Scene.h"
#include "subsystems/AssetManager.h"
#include "subsystems/Log.h"
#include "subsystems/RenderSubsystem.h"
#include "subsystems/ScriptingSubsystem.h"
#include "tools/Profiler.h"


using namespace Blainn;

eastl::shared_ptr<vgjs::JobSystem> Engine::m_jobSystemPtr = nullptr;

void Engine::Init()
{
    vgjs::thread_count_t jobSystemThreadCount{8};
    m_jobSystemPtr = eastl::make_shared<vgjs::JobSystem>(vgjs::JobSystem(jobSystemThreadCount));

    Log::Init();
    AssetManager::GetInstance().Init();
    ScriptingSubsystem::Init();

    // TODO: -- remove --  test asset manager
    auto a = AssetManager::GetInstance().LoadTexture(std::filesystem::current_path(), TextureType::ALBEDO);
}

void Engine::InitRenderSubsystem(HWND windowHandle)
{
    RenderSubsystem::Init();
}

void Engine::Destroy()
{
    RenderSubsystem::Destroy();
    ScriptingSubsystem::Destroy();
    AssetManager::GetInstance().Destroy();
    Log::Destroy();

    m_jobSystemPtr->terminate();
}

void Engine::Update(float deltaTime)
{
    /// ----- TEST SCRIPTING -----
    // Scene sc;
    // Entity entity = sc.CreateEntity();
    // entity.AddComponent<ScriptingComponent>();
    // uuid scriptUuid = ScriptingSubsystem::LoadScript(entity, "test1.lua").value();
    // ScriptingSubsystem::CallScriptFunction(scriptUuid, "abobus");
    // ScriptingSubsystem::UnloadScript(scriptUuid);
    // scriptUuid = ScriptingSubsystem::LoadScript(entity, "test2.lua").value();
    // ScriptingSubsystem::CallScriptFunction(scriptUuid, "OnUpdate");
    // ScriptingSubsystem::CallScriptFunction(scriptUuid, "OnCustomCall");
    // ScriptingSubsystem::UnloadScript(scriptUuid);
    /// ----- END TEST SCRIPTING -----

    // this trace doesn't make sense, it exactly matches the frame
    BLAINN_PROFILE_SCOPE_DYNAMIC("Main loop");

    // test
    static float testAccumulator;
    testAccumulator += deltaTime;
    if (testAccumulator >= 1000.0f)
    {
        std::cout << "Engine second" << std::endl;
        testAccumulator = 0.0f;
    }

    vgjs::schedule(&RenderSubsystem::Render);

    // Marks end of frame for tracy profiler
    BLAINN_PROFILE_MARK_FRAME;


    // TODO: wait for jobs to finish?
}