#include <cstdio>

#ifdef BLAINN_INCLUDE_EDITOR
#include <QApplication>
#include <QPushButton>
#include <pch.h>


#include "editor/include/Editor.h"
#else

#include "Render/UI/UIRenderer.h"

#endif

#include "engine/include/Engine.h"
#include "engine/include/tools/Timeline.h"

#define MAX_NAME_STRING 256
#define HInstance() GetModuleHandle(NULL)

INT WindowWidth;
INT WindowHeight;

CHAR WindowTitle[MAX_NAME_STRING];
CHAR WindowClass[MAX_NAME_STRING];

bool g_IsRunning = true;

int main(int argc, char **argv) noexcept
{
    try
    {
        BF_DEBUG("This is debug!");
        BF_ERROR("This is error!");
        BF_INFO("This is info!");
        BF_WARN("This is warn!");
        BF_TRACE("This is trace!");

#if !defined(BLAINN_INCLUDE_EDITOR)
    strcpy_s(WindowTitle, "Blainnflare Engine");
    strcpy_s(WindowClass, "D3D12SampleClass");

    WindowWidth = 1280;
    WindowHeight = 720;
#endif

    Blainn::Timeline<eastl::chrono::milliseconds> globalTimeline{nullptr};

    Blainn::Engine::Init(globalTimeline);
    HWND hwnd = NULL;

#if defined(BLAINN_INCLUDE_EDITOR)
    // needed for qt to generate resources (icons etc.)
    Q_INIT_RESOURCE(sources);

    Blainn::Editor::GetInstance().Init(argc, argv);
    Blainn::Editor::GetInstance().Show();

    hwnd = Blainn::Editor::GetInstance().GetViewportHWND();
#else
    hwnd = Blainn::Engine::CreateBlainnWindow(WindowWidth, WindowHeight, WindowTitle, WindowClass, HInstance());
#endif

    Blainn::Engine::InitRenderSubsystem(hwnd);

#if defined(BLAINN_INCLUDE_EDITOR)
    Blainn::Editor::GetInstance().PostInit();
#else
    Blainn::RenderSubsystem::GetInstance().SetEnableDebug(false);
    Blainn::RenderSubsystem::GetInstance().GetUIRenderer().ShouldRenderDebugUI = false;;
    Blainn::RenderSubsystem::GetInstance().OnResize(
        {static_cast<UINT>(WindowHeight), static_cast<UINT>(WindowWidth)});
    Blainn::Engine::StartPlayMode();
#endif

    MSG msg = {0};

    globalTimeline.Start();

    while (g_IsRunning)
    {
        // Process any messages in the queue.
        if (PeekMessage(&msg, NULL, 0u, 0u, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // fps stats could be added
            float mainTimelineDeltaTime = globalTimeline.Tick();
            Blainn::Engine::Update(mainTimelineDeltaTime);
#if defined(BLAINN_INCLUDE_EDITOR)
            Blainn::Editor::GetInstance().Update();
#endif

            if (msg.message == WM_QUIT)
            {
                g_IsRunning = false;
            }
        }
    }

#if defined(BLAINN_INCLUDE_EDITOR)
    Blainn::Editor::GetInstance().Destroy();
#endif
    Blainn::Engine::Destroy();

        return 0;
    }
    catch (const std::exception &e)
    {
        std::fputs("Fatal exception in main: ", stderr);
        std::fputs(e.what(), stderr);
        std::fputc('\n', stderr);
    }
    catch (...)
    {
        std::fputs("Fatal unknown exception in main\n", stderr);
    }

    return 1;
}
