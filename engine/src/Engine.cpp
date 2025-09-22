#include "Engine.h"

#include "VGJS.h"

#include "subsystems/Freya.h"
#include "subsystems/Log.h"
#include "tools/Profiler.h"

using namespace Blainn;

std::shared_ptr<vgjs::JobSystem> Engine::m_jobSystemPtr = nullptr;

void Engine::Init() {
	//TODO: Initialize engine subsystems here

    vgjs::thread_count_t jobSystemThreadCount{8};
    m_jobSystemPtr = std::make_shared<vgjs::JobSystem>(vgjs::JobSystem(jobSystemThreadCount));
    
    //   just to check that cmake is working
    Blainn::Freya::Init();
}

void Engine::Shutdown()
{
}

void Engine::Run()
{
    bool isRunning = true;
    while (isRunning)
    {
        // this trace doesn't make sense, it exactly matches the frame
        BLAINN_PROFILE_SCOPE_DYNAMIC("Main loop");

        volatile int sink = 0;
        for (int i = 0; i < 100; i++)
        {
            sink += i;
        }
        BF_DEBUG("This is written from the Engine::Run: {:>10}", sink);
        vgjs::schedule(&Freya::Render);

        // Marks end of frame for tracy profiler
        BLAINN_PROFILE_MARK_FRAME;
    }

}