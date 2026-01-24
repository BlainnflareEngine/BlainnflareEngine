#pragma once

#include "UploadBuffer.h"

namespace Blainn
{
    class Device;

    struct FrameResource
    {
        FrameResource(Device& device, UINT passCount, UINT materialCount, UINT maxNumPointLights, UINT maxNumSpotLights);
        FrameResource(const FrameResource &lhs) = delete;
        FrameResource &operator=(const FrameResource &lhs) = delete;

        ~FrameResource() noexcept;
        
        ComPtr<ID3D12CommandAllocator> commandAllocator;
        
        eastl::unique_ptr<UploadBuffer<PassConstants>> PassCB = nullptr;
        eastl::unique_ptr<UploadBuffer<MaterialData>> MaterialSB = nullptr;
        eastl::unique_ptr<UploadBuffer<PointLightInstanceData>> PointLightSB = nullptr;
        eastl::unique_ptr<UploadBuffer<SpotLightInstanceData>> SpotLightSB = nullptr;
        
        // Fence value to mark commands up to this fence point. This lets us check if these frame resources are still in use by the GPU.
        UINT64 Fence = 0;
    };
}