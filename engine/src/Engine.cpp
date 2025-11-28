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
#include "subsystems/RenderSubsystem.h"
#include "subsystems/ScriptingSubsystem.h"
#include "tools/Profiler.h"

using namespace Blainn;

eastl::shared_ptr<Device> Engine::m_device = nullptr;

void Engine::Init()
{
    vgjs::thread_count_t jobSystemThreadCount{8};
    s_JobSystemPtr = eastl::make_shared<vgjs::JobSystem>(vgjs::JobSystem(jobSystemThreadCount));

    SetDefaultContentDirectory();

    Log::Init();
    AssetManager::GetInstance().Init();
    ScriptingSubsystem::Init();

    // TODO: -- remove --  test asset manager
    auto a = AssetManager::GetInstance().LoadTexture(std::filesystem::current_path(), TextureType::ALBEDO);

    // TODO: -- remove -- test input
    Input::AddEventListener(InputEventType::KeyPressed,
                            [](const InputEventPointer &event)
                            {
                                const KeyPressedEvent *keyEvent = static_cast<const KeyPressedEvent *>(event.get());
                                BF_INFO("Key {} was pressed", static_cast<int>(keyEvent->GetKey()));
                            });

    Input::AddEventListener(InputEventType::KeyReleased,
                            [](const InputEventPointer &event)
                            {
                                const KeyReleasedEvent *keyEvent = static_cast<const KeyReleasedEvent *>(event.get());
                                BF_INFO("Key {} was released", static_cast<int>(keyEvent->GetKey()));
                            });
}

void Engine::InitRenderSubsystem(HWND windowHandle)
{
    #pragma region RenderingContext

#if defined(DEBUG) || defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    Device::CreateDebugLayer();
#endif
    m_device = eastl::make_shared<Device>();

#pragma endregion RenderingContext

    auto &renderInst = RenderSubsystem::GetInstance();
    renderInst.Init(m_device, windowHandle);
    m_renderFunc = std::bind(&RenderSubsystem::Render, &renderInst, std::placeholders::_1);
}

void Engine::Destroy()
{
    vgjs::wait_for_termination();

    ScriptingSubsystem::Destroy();
    AssetManager::GetInstance().Destroy();

    RenderSubsystem::GetInstance().Destroy();
    Log::Destroy();

    s_JobSystemPtr->terminate();
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

    std::counting_semaphore<1> updateDoneSem(0);


    Input::ProcessEvents();

    // test
    static float testAccumulator;
    testAccumulator += deltaTime;
    if (testAccumulator >= 1000.0f)
    {
        std::cout << "Engine second" << std::endl;

        // TODO: -- remove -- test input
        //Blainn::Input::UpdateKeyState(KeyCode::A, KeyState::Pressed);
        //Blainn::Input::UpdateKeyState(KeyCode::A, KeyState::Released);

        testAccumulator = 0.0f;
    }

    Scene::ProcessEvents();

    vgjs::schedule(
        [deltaTime, &updateDoneSem]() -> void
        {
            m_renderFunc(deltaTime);
            // std::cout << "render update" << std::endl;
            updateDoneSem.release();
        },
        vgjs::tag_t{1});
    vgjs::schedule(vgjs::tag_t{1});

    updateDoneSem.acquire();
    // std::cout << "loop done" << std::endl;

    // Marks end of frame for tracy profiler
    BLAINN_PROFILE_MARK_FRAME;
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

eastl::shared_ptr<Device> Engine::GetGraphicsDevice()
{
    return m_device;
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