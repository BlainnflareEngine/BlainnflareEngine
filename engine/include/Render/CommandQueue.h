#pragma once

#include "DXHelpers.h"
#include <mutex>

namespace Blainn
{
    class Direct3DQueue
    {
    public:
        Direct3DQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE commandType);
        ~Direct3DQueue();

        bool IsFenceComplete(UINT64 fenceValue);
        void InsertWait(UINT64 fenceValue);
        void InsertWaitForQueueFence(Direct3DQueue* otherQueue, UINT64 fenceValue);
        void InsertWaitForQueue(Direct3DQueue* otherQueue);

        void WaitForFenceCPUBlocking(UINT64 fenceValue);
        void WaitForIdle() { WaitForFenceCPUBlocking(m_nextFenceValue - 1); }

        ID3D12CommandQueue* GetCommandQueue() { return m_commandQueue; }

        UINT64 PollCurrentFenceValue();
        UINT64 GetLastCompletedFence() { return m_lastCompletedFenceValue; }
        UINT64 GetNextFenceValue() { return m_nextFenceValue; }
        ID3D12Fence* GetFence() { return m_fence; }

        UINT64 ExecuteCommandList(ID3D12CommandList* commandList);

    private:
        ID3D12CommandQueue* m_commandQueue;
        D3D12_COMMAND_LIST_TYPE m_queueType;

        std::mutex m_fenceMutex;
        std::mutex m_eventMutex;
        
        ID3D12Fence* m_fence;
        UINT64 m_nextFenceValue;
        UINT64 m_lastCompletedFenceValue;
        HANDLE m_fenceEventHandle;
    };
}