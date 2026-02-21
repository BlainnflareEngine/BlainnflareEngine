//
// Created by WhoLeb on 19-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//
#include "DescriptorHeap.h"

#include "pch.h"

#include "Render/backends/d3d12/DescriptorHeap.h"
#include "Render/Device.h"
#include "Render/DXHelpers.h"

using namespace Blainn;

DescriptorHeapAllocationManager::DescriptorHeapAllocationManager(
    Blainn::Device&                   _device,
    IDescriptorAllocator&             _parentAllocator,
    size_t                            _thisManagerId,
    const D3D12_DESCRIPTOR_HEAP_DESC& _heapDesc)
        : DescriptorHeapAllocationManager{
            _device,
            _parentAllocator,
            _thisManagerId,
            [&_heapDesc, &_device]()
            {
                ID3D12Device* pDevice = _device.GetDevice2().Get();
                ComPtr<ID3D12DescriptorHeap> pDescriptorHeap;
                pDevice->CreateDescriptorHeap(&_heapDesc, IID_PPV_ARGS(&pDescriptorHeap));
                return pDescriptorHeap.Get();
            }(),
            0,
            _heapDesc.NumDescriptors
        }
{
}

DescriptorHeapAllocationManager::DescriptorHeapAllocationManager(
    Device&               _device,
    IDescriptorAllocator& _parentAllocator,
    size_t                _thisManagerId,
    ID3D12DescriptorHeap* _pD3D12DescriptorHeap,
    uint32_t              _firstDescriptor,
    uint32_t              _numDescriptors)
        : m_ParentAllocator(_parentAllocator)
        , m_Device(_device)
        , m_ThisManagerId(_thisManagerId)
        , m_HeapDesc(_pD3D12DescriptorHeap->GetDesc())
        , m_DescriptorSize(_device.GetDescriptorHandleIncrementSize(m_HeapDesc.Type))
        , m_NumDescriptorsInAllocation(_numDescriptors)
        , m_FreeBlockManager(_numDescriptors)
        , m_pd3d12DescriptorHeap(_pD3D12DescriptorHeap)
{
    m_FirstCPUHandle = _pD3D12DescriptorHeap->GetCPUDescriptorHandleForHeapStart();
    m_FirstCPUHandle.ptr += static_cast<size_t>(m_DescriptorSize) * static_cast<size_t>(_firstDescriptor);
    if (m_HeapDesc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
    {
        m_FirstGPUHandle = _pD3D12DescriptorHeap->GetGPUDescriptorHandleForHeapStart();
        m_FirstGPUHandle.ptr += static_cast<size_t>(m_DescriptorSize) * static_cast<size_t>(_firstDescriptor);
    }
}

DescriptorHeapAllocationManager::~DescriptorHeapAllocationManager()
{}

DescriptorHeapAllocation DescriptorHeapAllocationManager::Allocate(uint32_t _count)
{
    BF_ASSERT_EXPR(_count > 0);
    std::lock_guard<std::mutex> lockGuard(m_FreeBlockManagerMutex);

    VariableSizeAllocationsManager::Allocation allocation = m_FreeBlockManager.Allocate(_count, 1);
    if (!allocation.IsValid())
        return DescriptorHeapAllocation{};

    BF_ASSERT_EXPR(allocation.Size == _count);

    D3D12_CPU_DESCRIPTOR_HANDLE CPUHandle = m_FirstCPUHandle;
    CPUHandle.ptr += allocation.UnalignedOffset * m_DescriptorSize;

    D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = m_FirstGPUHandle;
    if (m_HeapDesc.Flags & D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE)
        GPUHandle.ptr += allocation.UnalignedOffset * m_DescriptorSize;

    m_MaxAllocatedSize = std::max(m_MaxAllocatedSize, m_FreeBlockManager.GetUsedSize());

    BF_ASSERT(m_ThisManagerId < std::numeric_limits<uint16_t>::max(), "Manager exceeds 16-bit range");
    return DescriptorHeapAllocation{m_ParentAllocator, m_pd3d12DescriptorHeap.Get(),
                                    CPUHandle, GPUHandle, _count, static_cast<uint16_t>(m_ThisManagerId)};
}

void DescriptorHeapAllocationManager::FreeAllocation(DescriptorHeapAllocation &&_allocation)
{
    BF_ASSERT(_allocation.GetAllocationManagerId() == m_ThisManagerId, "Invalid descriptor heap manager id");

    if (_allocation.IsNull())
        return;

    std::lock_guard<std::mutex> lockGuard(m_FreeBlockManagerMutex);

    size_t descriptorOffset = (_allocation.GetCpuHandle().ptr - m_FirstCPUHandle.ptr) / m_DescriptorSize;
    m_FreeBlockManager.Free(descriptorOffset, _allocation.GetNumHandles());

    _allocation.Reset();
}

CPUDescriptorHeap::CPUDescriptorHeap(
    Device&                     _device,
    uint32_t                    _numDescriptorsInHeap,
    D3D12_DESCRIPTOR_HEAP_TYPE  _type,
    D3D12_DESCRIPTOR_HEAP_FLAGS _flags)
        : m_Device(_device)
        , m_HeapDesc
            {
                _type,
                _numDescriptorsInHeap,
                _flags,
                1
            }
        , m_DescriptorSize(_device.GetDescriptorHandleIncrementSize(m_HeapDesc.Type))
{
    m_HeapPool.emplace_back(_device, *this, 0, m_HeapDesc);
    m_AvailableHeaps.insert(0);
}

CPUDescriptorHeap::~CPUDescriptorHeap()
{
    BF_ASSERT(m_CurrentSize == 0, "Not all allocations were released");
    BF_ASSERT(m_AvailableHeaps.size() == m_HeapPool.size(), "Not all descriptor heap pools are released");

    uint32_t totalDescriptors = 0;
    for (DescriptorHeapAllocationManager& heap : m_HeapPool)
    {
        BF_ASSERT(heap.GetNumAvailableDescriptors() == heap.GetMaxDescriptors(), "Not all descriptors in the descriptor pool are released");
        totalDescriptors += heap.GetMaxDescriptors();
    }

    BF_INFO("{} CPU Heap allocated pool count: {}. Max Descriptors: {}/{} ({:.2f}%).",
        DescriptorHeapTypeToString(m_HeapDesc.Type),
        m_HeapPool.size(),
        m_MaxSize, totalDescriptors,
        m_MaxSize * 100.0 / static_cast<double>(std::max(totalDescriptors, 1u)));
}

DescriptorHeapAllocation CPUDescriptorHeap::Allocate(uint32_t _count)
{
    std::lock_guard<std::mutex> lockGuard(m_HeapPoolMutex);

    DescriptorHeapAllocation allocation;

    auto availableHeapIt = m_AvailableHeaps.begin();
    while (availableHeapIt != m_AvailableHeaps.end())
    {
        auto nexIt = availableHeapIt;
        ++nexIt;
        allocation = m_HeapPool[*availableHeapIt].Allocate(_count);

        if (m_HeapPool[*availableHeapIt].GetNumAvailableDescriptors() == 0)
            m_AvailableHeaps.erase(*availableHeapIt);

        if (!allocation.IsNull())
            break;
        availableHeapIt = nexIt;
    }

    if (allocation.IsNull())
    {
        if (_count > m_HeapDesc.NumDescriptors)
        {
            BF_INFO("Number of requested CPU descriptor handles ({}) exceeds the descriptor heap size ({}). Increasing the number of descriptors in the heap",
                _count, m_HeapDesc.NumDescriptors);
        }

        m_HeapDesc.NumDescriptors = std::max(m_HeapDesc.NumDescriptors, static_cast<uint32_t>(_count));
        m_HeapPool.emplace_back(m_Device, *this, m_HeapPool.size(), m_HeapDesc);
        auto newHeapIt = m_AvailableHeaps.insert(m_HeapPool.size() - 1);
        BF_ASSERT_EXPR(newHeapIt.second);

        allocation = m_HeapPool[*newHeapIt.first].Allocate(_count);
    }

    m_CurrentSize += static_cast<uint32_t>(allocation.GetNumHandles());
    m_MaxSize = std::max(m_MaxSize, m_CurrentSize);

    return allocation;
}

void CPUDescriptorHeap::Free(DescriptorHeapAllocation &&_allocation)
{
    std::lock_guard<std::mutex> lockGuard(m_HeapPoolMutex);
    size_t                      managerId = _allocation.GetAllocationManagerId();
    m_CurrentSize -= static_cast<uint32_t>(_allocation.GetNumHandles());
    m_HeapPool[managerId].FreeAllocation(std::move(_allocation));
    BF_ASSERT_EXPR(m_HeapPool[managerId].GetNumAvailableDescriptors() > 0);
    m_AvailableHeaps.insert(managerId);
}

void CPUDescriptorHeap::FreeAllocation(DescriptorHeapAllocation &&_allocation)
{
    (void)_allocation;
    BF_ASSERT(false, "Not implemented. Check out Diligent engine to see when this could be used");
}

GPUDescriptorHeap::GPUDescriptorHeap(
    Device&                     _device,
    uint32_t                    _numDescriptorsInHeap,
    // dynamic descriptors???
    D3D12_DESCRIPTOR_HEAP_TYPE  _type,
    D3D12_DESCRIPTOR_HEAP_FLAGS _flags)
        : m_Device(_device)
        , m_HeapDesc
        {
            _type,
            _numDescriptorsInHeap, // + dynamic descriptors???
            _flags,
            1
        }
        , m_pd3d12DescriptorHeap
        {
            [&]()
            {
                ComPtr<ID3D12DescriptorHeap> heap;
                _device.GetDevice2()->CreateDescriptorHeap(&m_HeapDesc, IID_PPV_ARGS(&heap));
                return heap;
            }()
        }
        , m_DescriptorSize(_device.GetDescriptorHandleIncrementSize(m_HeapDesc.Type))
        , m_HeapAllocationManager(_device, *this, 0, m_pd3d12DescriptorHeap.Get(), 0, _numDescriptorsInHeap)
{
}

GPUDescriptorHeap::~GPUDescriptorHeap()
{
    uint32_t totalSize = m_HeapAllocationManager.GetMaxDescriptors();
    uint32_t maxSize = m_HeapAllocationManager.GetMaxAllocatedSize();

    BF_INFO("{} GPU Heap max allocated size: {}/{} ({:.2f}%).",
        DescriptorHeapTypeToString(m_HeapDesc.Type),
        maxSize, totalSize,
        maxSize * 100.0 / static_cast<double>(std::max(totalSize, 1u)));
}

void GPUDescriptorHeap::Free(DescriptorHeapAllocation &&_allocation)
{
    if (_allocation.GetAllocationManagerId() == 0)
    {
        std::lock_guard<std::mutex> lockGuard(m_AllocationMutex);
        m_HeapAllocationManager.FreeAllocation(std::move(_allocation));
    }
}

