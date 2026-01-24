#include "Render/FrameResource.h"
#include "Render/Device.h"

Blainn::FrameResource::FrameResource(Device &device, UINT passCount, UINT materialCount, UINT maxNumPointLights, UINT maxNumSpotLights)
{
    ThrowIfFailed(device.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator));

    PassCB = eastl::make_unique<UploadBuffer<PassConstants>>(device.GetDevice2().Get(), passCount, TRUE);
    MaterialSB = eastl::make_unique<UploadBuffer<MaterialData>>(device.GetDevice2().Get(), materialCount, FALSE);                 // Structured buffer 
    PointLightSB = eastl::make_unique<UploadBuffer<PointLightInstanceData>>(device.GetDevice2().Get(), maxNumPointLights, FALSE); // Structured buffer
    SpotLightSB = eastl::make_unique<UploadBuffer<SpotLightInstanceData>>(device.GetDevice2().Get(), maxNumSpotLights, FALSE);    // Structured buffer
}

Blainn::FrameResource::~FrameResource()
{
}