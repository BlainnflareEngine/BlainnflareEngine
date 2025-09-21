#include "Engine.h"

#include "VGJS.h"

#include "subsystems/Freya.h"

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
        
        vgjs::schedule(&Freya::Render);
    }

}