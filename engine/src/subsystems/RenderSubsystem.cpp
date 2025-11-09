#include "subsystems/RenderSubsystem.h"

#include "Render/Device.h"
#include "Render/Renderer.h"

#include "subsystems/Log.h"
#include "tools/Profiler.h"

#include <cassert>
#include <iostream>

using namespace Blainn;

void Blainn::RenderSubsystem::Init() 
{
    if (m_isInitialized) return;

    BF_INFO("RenderSubsystem::Init()");

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    Blainn::Device::CreateDebugLayer();
#endif
    m_device = eastl::make_unique<Device>();
    m_renderer = eastl::make_unique<Renderer>(m_device.get(), m_width, m_height);
    
    m_renderer->Init();

    m_isInitialized = true;
}


void RenderSubsystem::Destroy()
{

}

RenderSubsystem& RenderSubsystem::GetInstance()
{
    static RenderSubsystem render;
    return render;
}

void RenderSubsystem::Render(float deltaTime)
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
    CreateRootSignature();
    CreateShaders();
    CreatePSO();
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
