#include "subsystems/Freya.h"

#include <iostream>
#include <cassert>

using namespace Blainn;

Freya::Freya()
{
    
}


Freya& Freya::GetInstance()
{
    assert(m_isInitialized && "Freya subsystem not initialized");
    static Freya instance;
    return instance;
}

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
}
