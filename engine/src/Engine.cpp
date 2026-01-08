#include "pch.h"

#include "Engine.h"

#include <VGJS.h>

#include <semaphore>

#include "Input/InputSubsystem.h"
#include "Input/KeyboardEvents.h"
#include "aliases.h"
#include "scene/Scene.h"
#include "subsystems/AssetManager.h"
#include "subsystems/Log.h"
#include "subsystems/PhysicsSubsystem.h"
#include "subsystems/RenderSubsystem.h"
#include "subsystems/ScriptingSubsystem.h"
#include "tools/Profiler.h"

using namespace Blainn;

void Engine::Init(Timeline<eastl::chrono::milliseconds> &globalTimeline)
{
#if defined(DEBUG) || defined(_DEBUG)
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
    AssetManager::GetInstance().Init();
    ScriptingSubsystem::Init();
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

    Input::ProcessEvents();

    // test
    static float testAccumulator;
    static int fpsCounterPrevValue;
    static int fpsCounter;
    fpsCounter++;
    testAccumulator += deltaTime;
    if (testAccumulator >= 1000.0f)
    {
        // std::cout << "Engine second" << std::endl;
        // BF_WARN("FPS: {}", fpsCounter - fpsCounterPrevValue);
        fpsCounterPrevValue = fpsCounter;

        testAccumulator -= 1000.0f;
    }

    if (s_isPlayMode)
    {
        float playModeDelta = s_playModeTimeline.Tick();
        PhysicsSubsystem::Update();
        ScriptingSubsystem::Update(*s_activeScene, playModeDelta / 1000.0f);
    }

    s_activeScene->Update();

    RenderSubsystem::GetInstance().Render(deltaTime);

    // Marks end of frame for tracy profiler
    BLAINN_PROFILE_MARK_FRAME;
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

    for (auto [entity, id, scriptComp] : s_activeScene->GetAllEntitiesWith<IDComponent, ScriptingComponent>().each())
    {
        for (auto [path, info] : scriptComp.scriptPaths)
            ScriptingSubsystem::LoadScript(s_activeScene->GetEntityWithUUID(id.ID), Path(path.c_str()),
                                           info.shouldTriggerStart);
    }
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

    for (auto [entity, id, scriptComp] : s_activeScene->GetAllEntitiesWith<IDComponent, ScriptingComponent>().each())
    {
        for (auto &[id, _] : scriptComp.scripts)
            ScriptingSubsystem::UnloadScript(id);
    }

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