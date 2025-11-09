#pragma once

struct ID3D12CommandQueue;
struct ID3D12Device;

namespace Blainn
{
    enum ECommandQueueType
    {
        GFX = 0,
        COMPUTE,
        COPY,

        NUM_COMMAND_QUEUE_TYPES = 3
    };

    class CommandQueue
    {
    public:
        void Create(ID3D12Device *pDevice, ECommandQueueType type, const char *name = nullptr);
        void Destroy();

        ID3D12CommandQueue *m_commandQueue = nullptr;
    };
} // namespace Blainn