#pragma once

#include "UploadBuffer.h"

namespace Blainn
{
    class Device;

    struct FrameResource
    {
        FrameResource(const eastl::shared_ptr<Device>& device, UINT passCount, UINT objectCount, UINT materialCount, UINT pointLightsCount);
        FrameResource(const FrameResource &lhs) = delete;
        FrameResource &operator=(const FrameResource &lhs) = delete;

        ~FrameResource();
        
        ComPtr<ID3D12CommandAllocator> commandAllocator;
        
        eastl::unique_ptr<UploadBuffer<ObjectConstants>> ObjectsCB = nullptr;
        eastl::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
        eastl::unique_ptr<UploadBuffer<MaterialData>> MaterialSB = nullptr;
        eastl::unique_ptr<UploadBuffer<InstanceData>> PointLightSB = nullptr;
        
        // Fence value to mark commands up to this fence point.  This lets us
        // check if these frame resources are still in use by the GPU.
        UINT64 Fence = 0;
    };
}