#include "Render/FrameResource.h"

Blainn::FrameResource::FrameResource(ID3D12Device *device, uint32_t passCount, uint32_t objectCount, uint32_t materialCount,
                             uint32_t pointLightsCount)
{
    ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(commandAllocator.GetAddressOf())));

    //ObjectsCB = eastl::make_unique<UploadBuffer<ObjectConstants>>(device, objectCount, TRUE);
    //PassCB = eastl::make_unique<UploadBuffer<PassConstants>>(device, passCount, TRUE);
    //MaterialSB = eastl::make_unique<UploadBuffer<MaterialData>>(device, materialCount, FALSE);      // Structured buffer
    //PointLightSB = eastl::make_unique<UploadBuffer<InstanceData>>(device, pointLightsCount, FALSE); // Structured buffer
}

Blainn::FrameResource::~FrameResource()
{
}