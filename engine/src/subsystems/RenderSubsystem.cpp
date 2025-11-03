#include "subsystems/RenderSubsystem.h"

#include "subsystems/Log.h"
#include "tools/Profiler.h"

#include <cassert>
#include <iostream>

using namespace Blainn;

void Blainn::RenderSubsystem::Init() 
{
    BF_INFO("RenderSubsystem::Init()");

#if !definded(BLAINN_INCLUDE_EDITOR)
    InitializeWindow();
#endif

    InitializeDirect3D();
}


void RenderSubsystem::Destroy()
{
}

void RenderSubsystem::Render()
{
    //BLAINN_PROFILE_THREAD("Render thread");
    assert(m_isInitialized && "Freya subsystem not initialized");

    BLAINN_PROFILE_SCOPE_DYNAMIC("Render function");

    volatile int sink = 0;
    for (int i = 0; i < 100; i++)
    {
        sink += i;
    }
    //BF_INFO("Haha, the number is {:>5}, and you can't count!", sink);
}

#pragma region GoingDirectX
void RenderSubsystem::InitializeDirect3D()
{
    CreateCommandObjects();
    
    CreateRootSignature();
    CreateShaders();   
    CreatePSO();

    m_isInitialized = true;
}
    
void RenderSubsystem::InitializeWindow()
{
    
}

void RenderSubsystem::CreateCommandObjects()
{

}
 
void RenderSubsystem::CreateRootSignature()
{

}

void RenderSubsystem::CreateShaders()
{

}

void RenderSubsystem::CreatePSO()
{

}

#pragma endregion GoingDirectX
