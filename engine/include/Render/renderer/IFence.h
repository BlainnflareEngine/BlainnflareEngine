//
// Created by WhoLeb on 02-Mar-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once

#include "IDeviceObject.h"

namespace Blainn
{
enum FENCE_TYPE : uint8_t
{
    /// Basic fence that may be used for:
    ///  - signaling the fence from GPU
    ///  - waiting for the fence on CPU
    FENCE_TYPE_CPU_WAIT_ONLY = 0,

    /// General fence that may be used for:
    ///  - signaling the fence from GPU
    ///  - waiting for the fence on CPU
    ///  - waiting for the fence on GPU
    ///
    /// If NativeFence feature is enabled (see DeviceFeatures), the fence may also be used for:
    ///  - signaling the fence on CPU
    ///  - waiting on GPU for a value that will be enqueued for signal later
    FENCE_TYPE_GENERAL = 1,

    FENCE_TYPE_LAST = FENCE_TYPE_GENERAL
};

struct FenceDesc : DeviceObjectAttribs
{
    FENCE_TYPE Type = FENCE_TYPE_CPU_WAIT_ONLY;
};

struct IFence : IDeviceObject
{
    virtual FenceDesc& GetDesc() const override = 0;

    /// Returns the last completed value signaled by the GPU

    /// \remarks   In Direct3D11 backend, this method is not thread-safe (even if the fence
    ///            object is protected by a mutex) and must only be called by the same thread
    ///            that signals the fence via IDeviceContext::EnqueueSignal().
    virtual uint64_t GetCompletedValue() = 0;

    /// Sets the fence to the specified value.

    /// \param [in] _value - New value to set the fence to.
    ///                     The value must be greater than the current value of the fence.
    ///
    /// Fence value will be changed immediately on the CPU.
    /// Use IDeviceContext::EnqueueSignal to enqueue a signal command
    /// that will change the value on the GPU after all previously submitted commands
    /// are complete.
    ///
    /// \note  The fence must have been created with type Diligent::FENCE_TYPE_GENERAL.
    virtual void Signal(uint64_t _value) = 0;


    /// Waits until the fence reaches or exceeds the specified value, on the host.

    /// \param [in] _value - The value that the fence is waiting for to reach.
    ///
    /// The method blocks the execution of the calling thread until the wait is complete.
    virtual void Wait(uint64_t _value) = 0;
};

} // namespace Blainn