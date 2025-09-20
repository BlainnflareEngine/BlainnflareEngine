#include "Engine.h"

#include "VGJS.h"

#include "subsystems/Freya.h"

using namespace Blainn;

void Engine::Init() {
	//TODO: Initialize engine subsystems here

    vgjs::JobSystem::init(8);
    
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


        // lets assume we have a render subsystem for now
        Blainn::Freya subsystem;
        vgjs::schedule(std::bind(&Freya::Render, &subsystem));
    }

}