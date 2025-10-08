#include "pch.h"
#include "Engine.h"

#include "VGJS.h"

#include "aliases.h"
#include "subsystems/RenderSubsystem.h"
#include "subsystems/Log.h"
#include "tools/Profiler.h"

#include "tools/Timeline.h"

using namespace Blainn;

std::shared_ptr<vgjs::JobSystem> Engine::m_jobSystemPtr = nullptr;

void Engine::Init() {
	//TODO: Initialize engine subsystems here

    vgjs::thread_count_t jobSystemThreadCount{8};
    m_jobSystemPtr = std::make_shared<vgjs::JobSystem>(vgjs::JobSystem(jobSystemThreadCount));
    
    //   just to check that cmake is working
    Blainn::RenderSubsystem::Init();

    Vec3 justToTest;
}

void Engine::Shutdown()
{
}

void Engine::Run()
{
    float marioPeriod =  1000.0 / 2.0;
    Timeline<eastl::chrono::milliseconds> sayMarioTimeline(marioPeriod);
    sayMarioTimeline.Start();

    Timeline<eastl::chrono::milliseconds> mainTimeline(1000.0 / 60.0);
    mainTimeline.Start();

    Timeline<eastl::chrono::milliseconds> physicsTimeline(m_physicsUpdatePeriodMs);
    physicsTimeline.Start();

    bool isRunning = true;
    while (isRunning)
    {
        float mainTimelineDeltaTime =  mainTimeline.Tick();
        float marioTimelineDeltaTime = sayMarioTimeline.Tick();
        static float marioAccumulator;
        marioAccumulator += mainTimelineDeltaTime;
        if (marioAccumulator >= marioPeriod)
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