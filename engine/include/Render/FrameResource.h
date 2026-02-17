#pragma once
#include <EASTL/unique_ptr.h>
#include <d3d12.h>

#include "UploadBuffer.h"

namespace Blainn
{
    class Device;

    struct FrameResourceCounts
    {
        UINT passCount = 0u;
        UINT materialCount = 0u;
        UINT maxNumPointLights = 0u;
        UINT maxNumSpotLights = 0u;
    };

    struct FrameResource
    {
        FrameResource(Device& device, const FrameResourceCounts& counts);
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
