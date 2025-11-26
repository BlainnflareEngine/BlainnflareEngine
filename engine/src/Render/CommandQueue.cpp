#pragma once

#include "Render/CommandQueue.h"

Blainn::CommandQueue::CommandQueue(const ComPtr<ID3D12Device2>& device, D3D12_COMMAND_LIST_TYPE type)
    : m_fenceValue(0)
    , m_commandListType(type)
    , m_device(device)
{
    D3D12_COMMAND_QUEUE_DESC desc = {};
    desc.Type = type;
    desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    desc.NodeMask = 0;

    ThrowIfFailed(m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue)));

    ThrowIfFailed(m_device->CreateFence(m_fenceValue,
                                             // D3D12_FENCE_FLAG_SHARED | D3D12_FENCE_FLAG_SHARED_CROSS_ADAPTER,
                                             D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

    m_fenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
    assert(m_fenceEvent && "Failed to create fence event handle.");
}

Blainn::CommandQueue::~CommandQueue()
{
    CloseHandle(m_fenceEvent);
}

UINT64 Blainn::CommandQueue::Signal()
{
    ++m_fenceValue;
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValue));
    return m_fenceValue;
}

bool Blainn::CommandQueue::IsFenceComplete(UINT64 fenceValue)
{
    /*
     * GetCompletedValue()
     * Returns the current value of the fence. If the device has been removed, the return value will be UINT64_MAX.
     */
    return fenceValue <= m_fence->GetCompletedValue();
}

void Blainn::CommandQueue::WaitForFenceValue(UINT64 fenceValue)
{
    if (!IsFenceComplete(fenceValue))
    {
        /*
         * SetEventOnCompletion(fenceValue, fenceEvent)
         * Specifies an event that's raised when the fence reaches a certain value.
         */
        ThrowIfFailed(m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent));
        WaitForSingleObject(m_fenceEvent, INFINITE); // WaitForSingleObjecEx(m_fenceEvent, INFINITE, FALSE)
    }
}

void Blainn::CommandQueue::Flush()
{
    WaitForFenceValue(Signal());
}

ComPtr<ID3D12CommandAllocator> Blainn::CommandQueue::CreateCommandAllocator()
{
    ComPtr<ID3D12CommandAllocator> commandAllocator;
    ThrowIfFailed(m_device->CreateCommandAllocator(m_commandListType, IID_PPV_ARGS(&commandAllocator)));

    return commandAllocator;
}

ComPtr<ID3D12GraphicsCommandList2> Blainn::CommandQueue::CreateCommandList(ID3D12CommandAllocator* pCommandAllocator)
{
    ComPtr<ID3D12GraphicsCommandList2> commandList;
    ThrowIfFailed(m_device->CreateCommandList(0u, m_commandListType, pCommandAllocator, nullptr, IID_PPV_ARGS(&commandList)));
    
    return commandList;
}

ComPtr<ID3D12GraphicsCommandList2> Blainn::CommandQueue::GetCommandList(ID3D12CommandAllocator *pCommandAllocator)
{
    ComPtr<ID3D12GraphicsCommandList2> commandList;

    if (!m_commandListQueue.empty())
    {
        commandList = m_commandListQueue.front();
        m_commandListQueue.pop();
    }
    else
    {
        commandList = CreateCommandList(pCommandAllocator);
        // We've created new command list, so it needs to be closed before reset
        ThrowIfFailed(commandList->Close());
    }

    ThrowIfFailed(commandList->Reset(pCommandAllocator, nullptr));

    return commandList;
}

// Execute a command list.
void Blainn::CommandQueue::ExecuteCommandList(ComPtr<ID3D12GraphicsCommandList2> commandList)
{
    ThrowIfFailed(commandList->Close());
    ID3D12CommandList *const ppCommandLists[] = {commandList.Get()};
    m_commandQueue->ExecuteCommandLists(1u, ppCommandLists);
    
    m_commandListQueue.push(commandList);
}

ComPtr<ID3D12CommandQueue> Blainn::CommandQueue::GetCommandQueue() const
{
    return m_commandQueue;
}