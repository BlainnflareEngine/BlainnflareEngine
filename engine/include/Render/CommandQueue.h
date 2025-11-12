#pragma once

#include "Render/Device.h"
#include "DXHelpers.h"

struct ID3D12CommandQueue;
struct ID3D12Fence;

namespace Blainn
{
    class CommandQueue
    {
    public:
        CommandQueue(eastl::shared_ptr<Device>, D3D12_COMMAND_LIST_TYPE type);
        ~CommandQueue();

        ID3D12CommandQueue *Get() const;

    private:

        ComPtr<ID3D12CommandQueue> m_commandQueue = nullptr;
    };
} // namespace Blainn