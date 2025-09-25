#include "subsystems/Freya.h"

#include "subsystems/Log.h"
#include "tools/Profiler.h"

#include <cassert>
#include <iostream>

using namespace Blainn;

bool Freya::m_isInitialized = false;

void Blainn::Freya::Init() 
{
    m_isInitialized = true;
    BF_INFO("RenderSubsystem::Init()");
}


void Freya::Destroy()
{
}

void Freya::Render()
{
    //BLAINN_PROFILE_THREAD("Render thread");
    assert(m_isInitialized && "Freya subsystem not initialized");

    BLAINN_PROFILE_SCOPE_DYNAMIC("Render function");

    volatile int sink = 0;
    for (int i = 0; i < 100; i++)
    {
        sink += i;
    }
    BF_INFO("Haha, the number is {:>5}, and you can't count!", sink);
}
