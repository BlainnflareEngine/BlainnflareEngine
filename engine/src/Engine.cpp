#include "pch.h"

#include "Engine.h"

#include "ComponentRegistry.h"

#pragma warning(push)
#pragma warning(disable : 4100)
#include <VGJS.h>
#pragma warning(pop)

#include <windowsx.h>

#include "Input/InputSubsystem.h"
#include "Input/KeyboardEvents.h"
#include "Input/MouseEvents.h"
#include "Navigation/NavigationSubsystem.h"
#include "Render/UI/UIRenderer.h"
#include "aliases.h"
#include "scene/Scene.h"
#include "subsystems/AISubsystem.h"
#include "subsystems/AssetManager.h"
#include "subsystems/Log.h"
#include "subsystems/PerceptionSubsystem.h"
#include "subsystems/PhysicsSubsystem.h"
#include "subsystems/RenderSubsystem.h"
#include "subsystems/ScriptingSubsystem.h"
#include "tools/Profiler.h"

using namespace Blainn;

void Engine::Init(Timeline<eastl::chrono::milliseconds> &globalTimeline)
{
    InitializeComponentRegistry();

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

    AssetManager::GetInstance().Init();
    ScriptingSubsystem::Init();

    InitAISubsystem();

#ifndef BLAINN_INCLUDE_EDITOR
    if (!AssetManager::SceneExists(s_config.GetDefaultSceneName()))
        AssetManager::CreateScene(s_config.GetDefaultSceneName());

    AssetManager::OpenScene(s_config.GetDefaultSceneName());
#endif
}

void Engine::InitAISubsystem()
{
    PerceptionSubsystem::Settings perceptionSettings;
    PerceptionSubsystem::GetInstance().Init(perceptionSettings);

    AISubsystem::Settings aiSettings;
    AISubsystem::GetInstance().Init(aiSettings);

    NavigationSubsystem::Init();
    NavigationSubsystem::SetShouldDrawDebug(true);

    PhysicsSubsystem::AddEventListener(
        PhysicsEventType::CollisionStarted,
        [](const eastl::shared_ptr<PhysicsEvent> &event)
        {
            auto &sceneManager = GetSceneManager();
            auto entity1 = sceneManager.TryGetEntityWithUUID(event->entity1);
            auto entity2 = sceneManager.TryGetEntityWithUUID(event->entity2);

            if (!entity1.IsValid() || !entity2.IsValid()) return;

            eastl::string tag1 = "Unknown";
            eastl::string tag2 = "Unknown";

            if (entity1.HasComponent<StimulusComponent>() && entity2.HasComponent<PerceptionComponent>())
            {
                Vec3 pos1 = sceneManager.GetWorldSpaceTransform(entity1).GetTranslation();
                bool touch = entity2.GetComponent<PerceptionComponent>().enableTouch;
                if (touch)
                {
                    tag1 = entity1.GetComponent<StimulusComponent>().tag;
                    PerceptionSubsystem::GetInstance().RegisterStimulus(entity1.GetUUID(), StimulusType::Touch, pos1,
                                                                        0.0f, tag1);
                }
            }
            if (entity2.HasComponent<StimulusComponent>() && entity1.HasComponent<PerceptionComponent>())
            {
                Vec3 pos2 = sceneManager.GetWorldSpaceTransform(entity2).GetTranslation();
                bool touch = entity1.GetComponent<PerceptionComponent>().enableTouch;
                if (touch)
                {
                    tag2 = entity2.GetComponent<StimulusComponent>().tag;
                    PerceptionSubsystem::GetInstance().RegisterStimulus(entity2.GetUUID(), StimulusType::Touch, pos2,
                                                                        0.0f, tag2);
                }
            }
        });
}

void Engine::InitRenderSubsystem(HWND windowHandle)
{
    auto &renderInst = RenderSubsystem::GetInstance();
    renderInst.Init(windowHandle);
    s_renderFunc = std::bind(&RenderSubsystem::Render, &renderInst, std::placeholders::_1);
}

void Engine::Destroy()
{
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

    s_sceneManager.CloseScenes();
}

void Engine::Update(float deltaTime)
{
    // this trace doesn't make sense, it exactly matches the frame
    BLAINN_PROFILE_SCOPE_DYNAMIC("Engine loop");

    s_deltaTime = deltaTime;
    RenderSubsystem::GetInstance().GetUIRenderer().StartImGuiFrame();

    Input::ProcessEvents();

    float playModeDelta = s_playModeTimeline.Tick() / 1000.0f;

    if (s_isPlayMode && !s_playModePaused)
    {
        if (s_sceneManager.GetActiveScene())
            ScriptingSubsystem::Update(*s_sceneManager.GetActiveScene(), playModeDelta);

        for (auto &[id, scene] : s_sceneManager.GetAdditiveScenes())
            ScriptingSubsystem::Update(*scene, playModeDelta);

        PhysicsSubsystem::Update(playModeDelta);
        PerceptionSubsystem::GetInstance().Update(playModeDelta);
        NavigationSubsystem::Update(playModeDelta);
        AISubsystem::GetInstance().Update(playModeDelta);
    }

    s_sceneManager.UpdateScenes();
    s_sceneManager.ProcessLocalEvents();
    SceneManager::ProcessStaticEvents();

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
    if (!s_sceneManager.GetActiveScene()) return;

    s_sceneManager.GetActiveScene()->SaveScene();

    InitScenePlayMode();
}

void Engine::TogglePausePlayMode()
{
    if (!s_playModePaused)
    {
        s_playModePaused = true;
    }
    else
    {
        s_playModePaused = false;
    }
}


void Engine::EscapePlayMode()
{
    if (!s_isPlayMode) return;

    if (s_sceneManager.GetActiveScene())
    {
        s_sceneManager.EndPlayMode();
    }

    AssetManager::GetInstance().OpenScene(s_startPlayModeSceneName.c_str());

    s_isPlayMode = false;
    AssetManager::GetInstance().ResetTextures();
}


bool Engine::IsPlayMode()
{
    return s_isPlayMode;
}


bool Engine::PlayModePaused()
{
    return s_playModePaused;
}

void Engine::InitScenePlayMode()
{
    if (!s_sceneManager.GetActiveScene()) return;

    s_sceneManager.StartPlayMode();

    if (s_sceneManager.GetActiveScene()) s_startPlayModeSceneName = s_sceneManager.GetActiveScene()->GetName();

    s_playModeTimeline.Reset();
    s_playModeTimeline.Start();
    s_isPlayMode = true;

    PhysicsSubsystem::StartSimulation();

    if (s_sceneManager.GetActiveScene())
    {
        for (auto [entity, id, aiComp] :
             s_sceneManager.GetActiveScene()->GetAllEntitiesWith<IDComponent, AIControllerComponent>().each())
        {
            Entity ent = s_sceneManager.GetActiveScene()->GetEntityWithUUID(id.ID);
            AISubsystem::GetInstance().CreateAIController(ent);
        }

        ScriptingSubsystem::LoadAllScripts(*s_sceneManager.GetActiveScene());
    }

    for (auto &[id, scene] : s_sceneManager.GetAdditiveScenes())
    {
        for (auto [entity, entityId, aiComp] : scene->GetAllEntitiesWith<IDComponent, AIControllerComponent>().each())
        {
            Entity ent = scene->GetEntityWithUUID(entityId.ID);
            AISubsystem::GetInstance().CreateAIController(ent);
        }

        ScriptingSubsystem::LoadAllScripts(*scene);
    }
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


EngineConfig &Engine::GetConfig()
{
    return s_config;
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
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
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
        Input::UpdateKeyState(static_cast<KeyCode>(wParam), KeyState::Pressed);
        if (wParam == VK_ESCAPE) PostQuitMessage(0);
        return 0;
    case WM_KEYUP:
        Input::UpdateKeyState(static_cast<KeyCode>(wParam), KeyState::Released);
        return 0;

    case WM_LBUTTONDOWN:
    {
        int mouseX = GET_X_LPARAM(lParam);
        int mouseY = GET_Y_LPARAM(lParam);
        Input::ResetMousePosition(static_cast<float>(mouseX), static_cast<float>(mouseY));
        Input::UpdateButtonState(MouseButton::Left, ButtonState::Pressed);
        SetCapture(hwnd);
        return 0;
    }
    case WM_LBUTTONUP:
    {
        int mouseX = GET_X_LPARAM(lParam);
        int mouseY = GET_Y_LPARAM(lParam);
        Input::ResetMousePosition(static_cast<float>(mouseX), static_cast<float>(mouseY));
        Input::UpdateButtonState(MouseButton::Left, ButtonState::Released);
        ReleaseCapture();
        return 0;
    }
    case WM_RBUTTONDOWN:
    {
        int mouseX = GET_X_LPARAM(lParam);
        int mouseY = GET_Y_LPARAM(lParam);
        Input::ResetMousePosition(static_cast<float>(mouseX), static_cast<float>(mouseY));
        Input::UpdateButtonState(MouseButton::Right, ButtonState::Pressed);
        SetCapture(hwnd);
        return 0;
    }
    case WM_RBUTTONUP:
    {
        int mouseX = GET_X_LPARAM(lParam);
        int mouseY = GET_Y_LPARAM(lParam);
        Input::ResetMousePosition(static_cast<float>(mouseX), static_cast<float>(mouseY));
        Input::UpdateButtonState(MouseButton::Right, ButtonState::Released);
        ReleaseCapture();
        return 0;
    }
    case WM_MBUTTONDOWN:
    {
        int mouseX = GET_X_LPARAM(lParam);
        int mouseY = GET_Y_LPARAM(lParam);
        Input::ResetMousePosition(static_cast<float>(mouseX), static_cast<float>(mouseY));
        Input::UpdateButtonState(MouseButton::Middle, ButtonState::Pressed);
        SetCapture(hwnd);
        return 0;
    }
    case WM_MBUTTONUP:
    {
        int mouseX = GET_X_LPARAM(lParam);
        int mouseY = GET_Y_LPARAM(lParam);
        Input::ResetMousePosition(static_cast<float>(mouseX), static_cast<float>(mouseY));
        Input::UpdateButtonState(MouseButton::Middle, ButtonState::Released);
        ReleaseCapture();
        return 0;
    }

    case WM_MOUSEMOVE:
    {
        int mouseX = GET_X_LPARAM(lParam);
        int mouseY = GET_Y_LPARAM(lParam);
        Input::UpdateMousePosition(static_cast<float>(mouseX), static_cast<float>(mouseY));
        return 0;
    }

    case WM_MOUSEWHEEL:
    {
        short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        Input::UpdateScrollState(0.0f, static_cast<float>(wheelDelta));
        return 0;
    }
    case WM_MOUSEHWHEEL:
    {
        short wheelDelta = GET_WHEEL_DELTA_WPARAM(wParam);
        Input::UpdateScrollState(static_cast<float>(wheelDelta), 0.0f);
        return 0;
    }
    case WM_SIZE:
    {
        UINT width = LOWORD(lParam);
        UINT height = HIWORD(lParam);
        RenderSubsystem::GetInstance().OnResize(RenderSubsystem::ResizeParams{width, height});
        return 0;
    }

    default:
        return DefWindowProc(hwnd, msg, wParam, lParam);
    }
}
