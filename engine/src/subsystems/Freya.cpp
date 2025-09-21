#include "subsystems/Freya.h"

#include "tools/Profiler.h"

#include <cassert>
#include <iostream>

using namespace Blainn;

bool Freya::m_isInitialized = false;

void Blainn::Freya::Init() 
{
    m_isInitialized = true;
    std::cout << "RenderSubsystem::Init()" << std::endl;
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
    std::cout << sink << '\n';
}
