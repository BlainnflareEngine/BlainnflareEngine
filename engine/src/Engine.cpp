#include "Engine.h"
#include "pch.h"
#include "VGJS.h"

#include "aliases.h"
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
}

void Engine::InitRenderSubsystem(HWND windowHandle)
{
    auto &renderInst = RenderSubsystem::GetInstance();

    renderInst.Init(windowHandle);
    m_renderFunc = std::bind(&RenderSubsystem::Render, &renderInst, std::placeholders::_1);
}

void Engine::Destroy()
{
    ScriptingSubsystem::Destroy();
    AssetManager::GetInstance().Destroy();
    
    m_JobSystemPtr->terminate();

    RenderSubsystem::GetInstance().Destroy();
    Log::Destroy();

}

void Engine::Update(float deltaTime)
{
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

    vgjs::schedule([deltaTime]() -> void
        {
            m_renderFunc(deltaTime);
        });

    // Marks end of frame for tracy profiler
    BLAINN_PROFILE_MARK_FRAME;


    // TODO: wait for jobs to finish?
}

HWND Engine::CreateBlainnWindow(UINT width, UINT height, const std::string &winTitle, const std::string &winClassTitle, HINSTANCE hInst)
{
    DWORD winStyle = WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME | WS_MINIMIZEBOX | WS_MAXIMIZEBOX;

    //// Load the icon
    //HANDLE icon = LoadImageA(nullptr, _PROJECT_DIR_ "\\nvidia.ico", IMAGE_ICON, 0, 0,
    //                         LR_LOADFROMFILE | LR_DEFAULTSIZE | LR_SHARED);

    // Register the window class
    WNDCLASSEX wc = { 0 };
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInst;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = winClassTitle.c_str();
    //wc.hIcon = (HICON)icon;

    RegisterClassEx(&wc);

    RECT windowRect = {0, 0, static_cast<LONG>(width), static_cast<LONG>(height)};
    AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

    // create the window
    HWND hWnd = CreateWindow(
        wc.lpszClassName, 
        winTitle.c_str(),
        winStyle,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        windowRect.right - windowRect.left,
        windowRect.bottom - windowRect.top, 
        nullptr, 
        nullptr, 
        wc.hInstance,
        nullptr);

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