//
// Created by WhoLeb on 01-Mar-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once

#include "IDeviceObject.h"
#include "Render/GraphicsTypes.h"
#include "Render/memory/IDataBlob.h"

namespace Blainn
{

enum PSO_CACHE_MODE : uint8_t
{
    /// PSO cache will be used to load PSOs from it.
    PSO_CACHE_MODE_LOAD  = 1u << 0u,

    /// PSO cache will be used to store PSOs.
    PSO_CACHE_MODE_STORE = 1u << 1u,

    /// PSO cache will be used to load and store PSOs.
    PSO_CACHE_MODE_LOAD_STORE = PSO_CACHE_MODE_LOAD | PSO_CACHE_MODE_STORE
};
DEFINE_FLAG_ENUM_OPERATORS(PSO_CACHE_MODE);


/// Pipeline state cache flags.
enum PSO_CACHE_FLAGS : uint8_t
{
    /// No flags.
    PSO_CACHE_FLAG_NONE = 0u,

    /// Print diagnostic messages e.g. when PSO is not found in the cache.
    PSO_CACHE_FLAG_VERBOSE = 1u << 0u
};
DEFINE_FLAG_ENUM_OPERATORS(PSO_CACHE_FLAGS);

struct PipelineStateCacheDesc : DeviceObjectAttribs
{
    /// Cache mode, see PSO_CACHE_MODE.

    /// Metal backend allows generating the cache on one device
    /// and loading PSOs from it on another.
    ///
    /// Vulkan PSO cache depends on the GPU device, driver version and other parameters,
    /// so the cache must be generated and used on the same device.
    PSO_CACHE_MODE Mode   = PSO_CACHE_MODE_LOAD_STORE;

    PSO_CACHE_FLAGS Flags = PSO_CACHE_FLAG_NONE;
};

struct PipelineStateCacheCreateInfo
{
    PipelineStateCacheDesc Desc;

    const void* pCacheData    = nullptr;
    uint32_t    CacheDataSize = 0;
};

struct IPipelineStateCache : IDeviceObject
{
    virtual void GetData(IDataBlob** ppBlob) = 0;
};
} // namespace Blainn

