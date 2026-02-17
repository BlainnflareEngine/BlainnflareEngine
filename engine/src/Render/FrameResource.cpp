#include "Render/FrameResource.h"
#include "Render/Device.h"

Blainn::FrameResource::FrameResource(Device &device, const FrameResourceCounts& counts)
{
    ThrowIfFailed(device.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator));

    PassCB = eastl::make_unique<UploadBuffer<PassConstants>>(device.GetDevice2().Get(), counts.passCount, TRUE);
    MaterialSB = eastl::make_unique<UploadBuffer<MaterialData>>(device.GetDevice2().Get(), counts.materialCount, FALSE);                 // Structured buffer 
    PointLightSB = eastl::make_unique<UploadBuffer<PointLightInstanceData>>(device.GetDevice2().Get(), counts.maxNumPointLights, FALSE); // Structured buffer
    SpotLightSB = eastl::make_unique<UploadBuffer<SpotLightInstanceData>>(device.GetDevice2().Get(), counts.maxNumSpotLights, FALSE);    // Structured buffer
}

Blainn::FrameResource::~FrameResource()
{
}
