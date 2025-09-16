#include "Engine.h"

#include "VGJS.h"

#include "subsystems/RenderSubsystem.h"

void Engine::Init() {
	//TODO: Initialize engine subsystems here

    vgjs::JobSystem::init(8);
    
    //   just to check that cmake is working
    Blainn::RenderSubsystem subsystem;
    subsystem.Init();
}

void Engine::Shutdown()
{
}

void Engine::Run()
{
    // lets assume we have a render subsystem for now
    Blainn::RenderSubsystem subsystem;
    vgjs::schedule(std::bind(&RenderSubsystem::ExecuteTask, &subsystem));
}