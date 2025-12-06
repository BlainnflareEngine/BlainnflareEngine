#include <iostream>

#ifdef BLAINN_INCLUDE_EDITOR
#include <QApplication>
#include <QPushButton>
#include <pch.h>


#include "editor/include/Editor.h"
#endif

#include "engine/include/Engine.h"
#include "engine/include/tools/Timeline.h"

#define MAX_NAME_STRING 256
#define HInstance() GetModuleHandle(NULL)

INT WindowWidth;
INT WindowHeight;

CHAR WindowTitle[MAX_NAME_STRING];
CHAR WindowClass[MAX_NAME_STRING];

int main(int argc, char **argv)
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

    bool isRunning = true;

    MSG msg = {0};

    globalTimeline.Start();

    while (isRunning)
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
                isRunning = false;
            }
        }
    }

#if defined(BLAINN_INCLUDE_EDITOR)
    Blainn::Editor::GetInstance().Destroy();
#endif
    Blainn::Engine::Destroy();

    return 0;
}