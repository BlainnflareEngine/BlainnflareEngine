#pragma once

#include "Render/MathHelper.h"
#include "Render/CommandQueue.h"

Blainn::Direct3DQueue::Direct3DQueue(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE commandType)
{
    m_queueType = commandType;
    m_commandQueue = nullptr;
    m_fence = nullptr;

    m_nextFenceValue = ((UINT64)m_queueType << 56) + 1;
    m_lastCompletedFenceValue = ((UINT64)m_queueType << 56);

    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    ZeroMemory(&queueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
    queueDesc.Type = m_queueType;
    queueDesc.Priority;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.NodeMask = 0u;
    // HRESULT
    device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));
    // HRESULT
    device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
    // HRESULT
    m_fence->Signal(m_lastCompletedFenceValue);
    // m_fenceEventHandle = CreateEventEx(NULL, false, false, EVENT_ALL_ACCESS);
}

Blainn::Direct3DQueue::~Direct3DQueue()
{
    CloseHandle(m_fenceEventHandle);

    m_fence->Release();
    m_fence = nullptr;

    m_commandQueue->Release();
    m_commandQueue = nullptr;
}

bool Blainn::Direct3DQueue::IsFenceComplete(UINT64 fenceValue)
{
    if (fenceValue > m_lastCompletedFenceValue)
    {
        PollCurrentFenceValue();
    }
    return fenceValue <= m_lastCompletedFenceValue;
}

void Blainn::Direct3DQueue::InsertWait(UINT64 fenceValue)
{
    // HRESULT
    m_commandQueue->Wait(m_fence, fenceValue);
}

void Blainn::Direct3DQueue::InsertWaitForQueueFence(Direct3DQueue* otherQueue, UINT64 fenceValue)
{
    // HRESULT
    m_commandQueue->Wait(otherQueue->GetFence(), fenceValue);
}

void Blainn::Direct3DQueue::InsertWaitForQueue(Direct3DQueue* otherQueue)
{
    // HRESULT
    m_commandQueue->Wait(otherQueue->GetFence(), otherQueue->GetNextFenceValue() - 1);
}

void Blainn::Direct3DQueue::WaitForFenceCPUBlocking(UINT64 fenceValue)
{
    if (IsFenceComplete(fenceValue)) return;
 
    {
        std::lock_guard<std::mutex> lockGuard(m_eventMutex);
        // HRESULT
        m_fence->SetEventOnCompletion(fenceValue, m_fenceEventHandle);
        WaitForSingleObjectEx(m_fenceEventHandle, INFINITE, false);
        m_lastCompletedFenceValue = fenceValue;
    }
}

UINT64 Blainn::Direct3DQueue::PollCurrentFenceValue()
{
    m_lastCompletedFenceValue = MathHelper::Max(m_lastCompletedFenceValue, m_fence->GetCompletedValue());
    return m_lastCompletedFenceValue;
}

UINT64 Blainn::Direct3DQueue::ExecuteCommandList(ID3D12CommandList* commandList)
{
    // HRESULT
    ((ID3D12GraphicsCommandList*)commandList)->Close();
    m_commandQueue->ExecuteCommandLists(1u, &commandList);
 
    std::lock_guard<std::mutex> lockGuard(m_fenceMutex);
 
    m_commandQueue->Signal(m_fence, m_nextFenceValue);
 
    return m_nextFenceValue++;
}