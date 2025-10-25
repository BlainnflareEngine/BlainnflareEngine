#include "Engine.h"
#include "pch.h"


#include "VGJS.h"

#include "aliases.h"
#include "subsystems/Log.h"
#include "subsystems/RenderSubsystem.h"
#include "subsystems/ScriptingSubsystem.h"
#include "tools/Profiler.h"


#include "tools/Timeline.h"

using namespace Blainn;

std::shared_ptr<vgjs::JobSystem> Engine::m_jobSystemPtr = nullptr;

void Engine::Init()
{
    // TODO: Initialize engine subsystems here

    vgjs::thread_count_t jobSystemThreadCount{8};
    m_jobSystemPtr = std::make_shared<vgjs::JobSystem>(vgjs::JobSystem(jobSystemThreadCount));

    //   just to check that cmake is working
    Blainn::RenderSubsystem::Init();

    Blainn::ScriptingSubsystem::Init();
}

void Engine::Shutdown()
{
}

void Engine::Run()
{
    Timeline<eastl::chrono::milliseconds> sayMarioTimeline;
    sayMarioTimeline.Start();

    Timeline<eastl::chrono::milliseconds> mainTimeline;
    mainTimeline.Start();

    bool isRunning = true;
    while (isRunning)
    {
        float mainTimelineDeltaTime = mainTimeline.Tick();
        float marioTimelineDeltaTime = sayMarioTimeline.Tick();
        static float marioAccumulator;
        marioAccumulator += mainTimelineDeltaTime;
        if (marioAccumulator >= 500.0f)
        {
            std::cout << "Hello, it's me, Marrrriooooooo!" << std::endl;
            marioAccumulator = 0;
        }

        // this trace doesn't make sense, it exactly matches the frame
        BLAINN_PROFILE_SCOPE_DYNAMIC("Main loop");

        volatile int sink = 0;
        for (int i = 0; i < 100; i++)
        {
            sink += i;
        }
        // BF_DEBUG("This is written from the Engine::Run: {:>10}", sink);
        vgjs::schedule(&RenderSubsystem::Render);

        // Marks end of frame for tracy profiler
        BLAINN_PROFILE_MARK_FRAME;
    }
}