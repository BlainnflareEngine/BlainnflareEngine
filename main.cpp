#include <iostream>

#include <QApplication>
#include <QPushButton>
#include <pch.h>

#include "editor/include/Editor.h"
#include "engine/include/Engine.h"
#include "engine/include/tools/Timeline.h"

int main(int argc, char **argv)
{
    BF_DEBUG("This is debug!");
    BF_ERROR("This is error!");
    BF_INFO("This is info!");
    BF_WARN("This is warn!");
    BF_TRACE("This is trace!");

    Blainn::Timeline<eastl::chrono::milliseconds> globalTimeline{nullptr};

    Blainn::Engine::Init(globalTimeline);

#if defined(BLAINN_INCLUDE_EDITOR)
    // needed for qt to generate resources (icons etc.)
    Q_INIT_RESOURCE(sources);

    Blainn::Editor::GetInstance().Init(argc, argv);
    Blainn::Editor::GetInstance().Show();
#endif

    HWND hwnd = Blainn::Editor::GetInstance().GetViewportHWND();
    Blainn::Engine::InitRenderSubsystem(hwnd);


    globalTimeline.Start();

    bool isRunning = true;
    while (isRunning)
    {
        float mainTimelineDeltaTime = globalTimeline.Tick();
        Blainn::Engine::Update(mainTimelineDeltaTime);

#if defined(BLAINN_INCLUDE_EDITOR)
        Blainn::Editor::GetInstance().Update();
#endif
    }

    Blainn::Editor::GetInstance().Destroy();
    Blainn::Engine::Destroy();

    return 0;
}