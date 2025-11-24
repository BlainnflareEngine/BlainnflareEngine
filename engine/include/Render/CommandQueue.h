#pragma once

#include "Render/DXHelpers.h"

/**
 * Wrapper class for a ID3D12CommandQueue.
 */
    
namespace Blainn
{
    class CommandQueue
    {
    public:
        CommandQueue(ComPtr<ID3D12Device2> device, D3D12_COMMAND_LIST_TYPE type);
        /*virtual */~CommandQueue(); // gfx, compute, copy

        // Get an available command list from the command queue.
        ComPtr<ID3D12GraphicsCommandList2> GetCommandList(ID3D12CommandAllocator *pCommandList);

        // Execute a command list.
        void ExecuteCommandList(ComPtr<ID3D12GraphicsCommandList2> cmdList);

        UINT64 Signal();
        bool IsFenceComplete(UINT64 fenceValue);
        void WaitForFenceValue(UINT64 fenceValue);
        void Flush();

        ComPtr<ID3D12CommandQueue> GetCommandQueue() const;

    protected:
        ComPtr<ID3D12CommandAllocator> CreateCommandAllocator();
        ComPtr<ID3D12GraphicsCommandList2> CreateCommandList(ID3D12CommandAllocator *pCommandAllocator);

    private:
        using CommandListQueue = eastl::queue<ComPtr<ID3D12GraphicsCommandList2>>;

        ComPtr<ID3D12Device2> m_device;
        ComPtr<ID3D12CommandQueue> m_commandQueue;
        ComPtr<ID3D12Fence> m_fence;
        HANDLE m_fenceEvent;
        UINT64 m_fenceValue;

        D3D12_COMMAND_LIST_TYPE m_commandListType;
        CommandListQueue m_commandListQueue;
    };
} // namespace Blainn