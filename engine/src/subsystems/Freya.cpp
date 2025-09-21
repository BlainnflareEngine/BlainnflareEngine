#include "subsystems/Freya.h"

#include <iostream>
#include <cassert>

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
    assert(m_isInitialized && "Freya subsystem not initialized");
}
