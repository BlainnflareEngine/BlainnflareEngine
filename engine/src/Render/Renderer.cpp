#include "Render/Renderer.h"
#include "Render/Device.h"
#include "Render/CommandQueue.h"

void Blainn::Renderer::Init()
{
#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    Blainn::Device::CreateDebugLayer();
#endif
    
}