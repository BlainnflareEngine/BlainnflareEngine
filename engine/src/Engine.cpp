#include "pch.h"

#include "Engine.h"

#include <VGJS.h>

#include <semaphore>

#include "Input/InputSubsystem.h"
#include "Input/KeyboardEvents.h"
#include "aliases.h"
#include "Input/MouseEvents.h"
#include "Navigation/NavigationSubsystem.h"
#include "scene/Scene.h"
#include "subsystems/AssetManager.h"
#include "subsystems/Log.h"
#include "subsystems/PhysicsSubsystem.h"
#include "subsystems/RenderSubsystem.h"
#include "subsystems/ScriptingSubsystem.h"
#include "subsystems/AISubsystem.h"
#include "subsystems/PerceptionSubsystem.h"
#include "tools/Profiler.h"

using namespace Blainn;

void Engine::Init(Timeline<eastl::chrono::milliseconds> &globalTimeline)
{
#if (defined(DEBUG) || defined(_DEBUG)) && !defined(BLAINN_DISABLE_D3D_DEBUG_LAYER)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    Device::CreateDebugLayer();
#endif
    bool useWarpDevice = false;
    Device::GetInstance().Init(useWarpDevice);

    vgjs::thread_count_t jobSystemThreadCount{8};
    s_JobSystemPtr = eastl::make_shared<vgjs::JobSystem>(vgjs::JobSystem(jobSystemThreadCount));

    SetDefaultContentDirectory();

    Log::Init();
    RenderSubsystem::GetInstance().PreInit();
    PhysicsSubsystem::Init(globalTimeline);

    PerceptionSubsystem::GetInstance().Init();

    PerceptionSubsystem::Settings perceptionSettings;
    perceptionSettings.enableLOD = true;
    perceptionSettings.lodNearDistance = 1000.0f;
    perceptionSettings.lodMidDistance = 3000.0f;
    perceptionSettings.lodFarDistance = 5000.0f;
    perceptionSettings.lodNearUpdateInterval = 0.0f;
    perceptionSettings.lodMidUpdateInterval = 0.1f;
    perceptionSettings.lodFarUpdateInterval = 0.5f;
    PerceptionSubsystem::GetInstance().SetSettings(perceptionSettings);

    AssetManager::GetInstance().Init();
    ScriptingSubsystem::Init();
    AISubsystem::GetInstance().Init();

    AISubsystem::Settings aiSettings;
    aiSettings.enableLOD = true;
    aiSettings.lodNearDistance = 1000.0f;
    aiSettings.lodMidDistance = 3000.0f;
    aiSettings.lodFarDistance = 5000.0f;
    aiSettings.lodNearUpdateInterval = 0.0f;
    aiSettings.lodMidUpdateInterval = 0.1f;
    aiSettings.lodFarUpdateInterval = 0.5f;
    AISubsystem::GetInstance().SetSettings(aiSettings);

    NavigationSubsystem::Init();
    NavigationSubsystem::SetShouldDrawDebug(true);

    PhysicsSubsystem::AddEventListener(PhysicsEventType::CollisionStarted,
                                       [](const eastl::shared_ptr<PhysicsEvent> &event)
                                       {
                                           Scene &scene = *Engine::GetActiveScene();
                                           auto entity1 = scene.GetEntityWithUUID(event->entity1);
                                           auto entity2 = scene.GetEntityWithUUID(event->entity2);

                                           if (!entity1.IsValid() || !entity2.IsValid()) return;

                                           Vec3 pos1 = scene.GetWorldSpaceTransform(entity1).GetTranslation();
                                           Vec3 pos2 = scene.GetWorldSpaceTransform(entity2).GetTranslation();

                                           eastl::string tag1 = "Unknown";
                                           eastl::string tag2 = "Unknown";

                                           if (entity1.HasComponent<StimulusComponent>())
                                               tag1 = entity1.GetComponent<StimulusComponent>().tag;
                                           if (entity2.HasComponent<StimulusComponent>())
                                               tag2 = entity2.GetComponent<StimulusComponent>().tag;

                                           PerceptionSubsystem::GetInstance().RegisterStimulus(
                                               entity2.GetUUID(), StimulusType::Touch, pos1, 0.0f, tag2);
                                           PerceptionSubsystem::GetInstance().RegisterStimulus(
                                               entity1.GetUUID(), StimulusType::Touch, pos2, 0.0f, tag1);
                                       });
}

void Engine::InitRenderSubsystem(HWND windowHandle)
{
    auto &renderInst = RenderSubsystem::GetInstance();
    renderInst.Init(windowHandle);
    m_renderFunc = std::bind(&RenderSubsystem::Render, &renderInst, std::placeholders::_1);
}

void Engine::Destroy()
{
    s_activeScene = nullptr;

    NavigationSubsystem::Destroy();
    AISubsystem::GetInstance().Destroy();
    PerceptionSubsystem::GetInstance().Destroy();
    ScriptingSubsystem::Destroy();
    AssetManager::GetInstance().Destroy();

    RenderSubsystem::GetInstance().Destroy();
    PhysicsSubsystem::Destroy();
    Log::Destroy();

    Device::GetInstance().Destroy();

    s_JobSystemPtr->terminate();
}

void Engine::Update(float deltaTime)
{
    // this trace doesn't make sense, it exactly matches the frame
    BLAINN_PROFILE_SCOPE_DYNAMIC("Main loop");

    s_deltaTime = deltaTime;

    Input::ProcessEvents();

    if (s_isPlayMode)
    {
        float playModeDelta = s_playModeTimeline.Tick() / 1000.0f;

        ScriptingSubsystem::Update(*s_activeScene, playModeDelta);
        PhysicsSubsystem::Update(playModeDelta);
        PerceptionSubsystem::GetInstance().Update(playModeDelta);
        AISubsystem::GetInstance().Update(playModeDelta);
        NavigationSubsystem::Update(playModeDelta);
    }

    s_activeScene->Update();

    if (NavigationSubsystem::ShouldDrawDebug()) NavigationSubsystem::DrawDebugMesh();

    RenderSubsystem::GetInstance().Render(deltaTime);

    // Marks end of frame for tracy profiler
    BLAINN_PROFILE_MARK_FRAME;
}


float Engine::GetDeltaTime()
{
    return s_deltaTime;
}


void Engine::StartPlayMode()
{
    if (s_isPlayMode) return;
    if (!s_activeScene) return;

    s_activeScene->StartPlayMode();
    s_activeScene->SaveScene();

    s_playModeTimeline.Reset();
    s_playModeTimeline.Start();
    s_isPlayMode = true;

    PhysicsSubsystem::StartSimulation();

    for (auto [entity, id, aiComp] : s_activeScene->GetAllEntitiesWith<IDComponent, AIControllerComponent>().each())
    {
        Entity ent = s_activeScene->GetEntityWithUUID(id.ID);
        AISubsystem::GetInstance().CreateAIController(ent);
    }

    ScriptingSubsystem::LoadAllScripts(*s_activeScene);
}

void Engine::StopPlayMode()
{
    s_playModeTimeline.Pause();
    PhysicsSubsystem::StopSimulation();
}


void Engine::EscapePlayMode()
{
    if (!s_isPlayMode) return;
    if (s_activeScene)
    {
        s_activeScene->EndPlayMode();
        AssetManager::GetInstance().OpenScene(s_activeScene->GetName().c_str());
    }


    s_isPlayMode = false;

    PhysicsSubsystem::StopSimulation();
    ScriptingSubsystem::UnloadAllScripts(*s_activeScene);
    AssetManager::GetInstance().ResetTextures();
}


bool Engine::IsPlayMode()
{
    return s_isPlayMode;
}


Path &Engine::GetContentDirectory()
{
    return s_contentDirectory;
}


void Engine::SetContentDirectory(const Path &contentDirectory)
{
    s_contentDirectory = contentDirectory;
}


void Engine::SetDefaultContentDirectory()
{
    SetContentDirectory(std::filesystem::current_path() / "Content");
}


eastl::shared_ptr<Scene> Engine::GetActiveScene()
{
    return s_activeScene;
}


void Engine::SetActiveScene(const eastl::shared_ptr<Scene> &scene)
{
    s_activeScene = scene;
}

HWND Engine::CreateBlainnWindow(UINT width, UINT height, const std::string &winTitle, const std::string &winClassTitle,
                                HINSTANCE hInst)
{
    DWORD winStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

    //// Load the icon
    // HANDLE icon = LoadImageA(nullptr, _PROJECT_DIR_ "\\nvidia.ico", IMAGE_ICON, 0, 0,
    //                          LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);

    // Register the window class
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = winClassTitle.c_str();
    // wc.hIcon = (HICON)icon;

    RegisterClassEx(&wc);

    RECT windowRect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    // create the window
    HWND hWnd = CreateWindow(wc.lpszClassName, winTitle.c_str(), winStyle, CW_USEDEFAULT, CW_USEDEFAULT,
                             windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, nullptr, nullptr,
                             wc.hInstance, nullptr);

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    return hWnd;
}


LRESULT CALLBACK Engine::WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch (msg)
    {
    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    case WM_KEYDOWN:
        if (wParam == VK_ESCAPE) PostQuitMessage(0);
        return 0;
    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}


void Engine::ClearActiveScene()
{
    s_activeScene.reset();
}