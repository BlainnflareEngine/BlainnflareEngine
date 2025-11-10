#include "pch.h"

#include "Engine.h"

#include "VGJS.h"

#include "aliases.h"
#include "Input/InputSubsystem.h"
#include "Input/KeyboardEvents.h"
#include "scene/Scene.h"
#include "subsystems/AssetManager.h"
#include "subsystems/Log.h"
#include "subsystems/RenderSubsystem.h"
#include "subsystems/ScriptingSubsystem.h"
#include "tools/Profiler.h"


using namespace Blainn;

eastl::shared_ptr<vgjs::JobSystem> Engine::m_JobSystemPtr = nullptr;

void Engine::Init()
{
    vgjs::thread_count_t jobSystemThreadCount{8};
    m_JobSystemPtr = eastl::make_shared<vgjs::JobSystem>(vgjs::JobSystem(jobSystemThreadCount));

    Log::Init();
    AssetManager::GetInstance().Init();
    ScriptingSubsystem::Init();

    // TODO: -- remove --  test asset manager
    auto a = AssetManager::GetInstance().LoadTexture(std::filesystem::current_path(), TextureType::ALBEDO);

    // TODO: -- remove -- test input
    Input::AddEventListener(InputEventType::KeyPressed, [](const InputEventPointer& event)
    {
        const KeyPressedEvent* keyEvent = static_cast<const KeyPressedEvent*>(event.get());
        BF_INFO("Key {} was pressed", static_cast<int>(keyEvent->GetKey()));
    });

    Input::AddEventListener(InputEventType::KeyReleased, [](const InputEventPointer& event)
    {
        const KeyReleasedEvent* keyEvent = static_cast<const KeyReleasedEvent*>(event.get());
        BF_INFO("Key {} was released", static_cast<int>(keyEvent->GetKey()));
    });
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

    m_JobSystemPtr->terminate();
}

void Engine::Update(float deltaTime)
{
    // this trace doesn't make sense, it exactly matches the frame
    BLAINN_PROFILE_SCOPE_DYNAMIC("Main loop");

    /// ----- TEST SCRIPTING -----
    // Entity entity = sc.CreateEntity();
    // entity.AddComponent<ScriptingComponent>();
    // uuid scriptUuid = ScriptingSubsystem::LoadScript(entity, "test1.lua").value();
    // ScriptingSubsystem::CallScriptFunction(scriptUuid, "abobus");
    // ScriptingSubsystem::UnloadScript(scriptUuid);
    // scriptUuid = ScriptingSubsystem::LoadScript(entity, "test2.lua").value();
    // ScriptingSubsystem::CallScriptFunction(scriptUuid, "OnUpdate", 16.67f);
    // int b = 42;
    // ScriptingSubsystem::CallScriptFunction(scriptUuid, "OnCustomCall", eastl::ref(b));
    // std::cout << "Value of b after script call: " << b << std::endl;
    // ScriptingSubsystem::UnloadScript(scriptUuid);
    /// ----- END TEST SCRIPTING -----

    Input::ProcessEvents();

    // test
    static float testAccumulator;
    testAccumulator += deltaTime;
    if (testAccumulator >= 1000.0f)
    {
        std::cout << "Engine second" << std::endl;

        // TODO: -- remove -- test input
        Blainn::Input::UpdateKeyState(KeyCode::A, KeyState::Pressed);
        Blainn::Input::UpdateKeyState(KeyCode::A, KeyState::Released);

        testAccumulator = 0.0f;
    }

    vgjs::schedule(&RenderSubsystem::Render);

    // TODO: wait for jobs to finish?

    // Marks end of frame for tracy profiler
    BLAINN_PROFILE_MARK_FRAME;
}


Path &Engine::GetContentDirectory()
{
    return m_contentDirectory;
}


void Engine::SetContentDirectory(const Path &contentDirectory)
{
    m_contentDirectory = contentDirectory;
}


Path &Engine::GetContentDirectory()
{
    return m_contentDirectory;
}


void Engine::SetContentDirectory(const Path &contentDirectory)
{
    m_contentDirectory = contentDirectory;
}