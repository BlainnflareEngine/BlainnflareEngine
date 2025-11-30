#include "Render/FrameResource.h"
#include "Render/Device.h"

Blainn::FrameResource::FrameResource(Device &device, UINT passCount, UINT objectCount, UINT materialCount, UINT pointLightsCount)
{
    ThrowIfFailed(device.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator));

    ObjectsCB = eastl::make_unique<UploadBuffer<ObjectConstants>>(device.GetDevice2().Get(), objectCount, TRUE);
    PassCB = eastl::make_unique<UploadBuffer<PassConstants>>(device.GetDevice2().Get(), passCount, TRUE);
    // MaterialSB = eastl::make_unique<UploadBuffer<MaterialData>>(device.GetDevice2().Get(), materialCount, FALSE); //
    // Structured buffer PointLightSB = eastl::make_unique<UploadBuffer<InstanceData>>(device.GetDevice2().Get(),
    // pointLightsCount, FALSE); // Structured buffer
}

Blainn::FrameResource::~FrameResource()
{
}