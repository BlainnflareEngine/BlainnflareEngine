#pragma once

#include <cstddef>
#include <cstdint>
#include <d3d12.h>
#include <limits>
#include <wrl/client.h>

#include "helpers.h"
#include "Render/Device.h"
#include "Render/memory/VariableSizeAllocationsManager.h"

namespace Blainn
{
class DescriptorHeapAllocation;
class DescriptorHeapAllocationManager;

class IDescriptorAllocator
{
public:
    virtual DescriptorHeapAllocation Allocate(uint32_t _count)                                            = 0;
    virtual void                     Free(DescriptorHeapAllocation&& _allocation) = 0;
    virtual uint32_t                 GetDescriptorSize() const                                            = 0;

    virtual ~IDescriptorAllocator() = default;
};

class DescriptorHeapAllocation
{
public:
    DescriptorHeapAllocation() noexcept
        : m_pDescriptorHeap(nullptr)
        , m_NumHandles     (1      )
        , m_DescriptorSize (0      )
    {
        m_FirstCpuHandle.ptr = 0;
        m_FirstGpuHandle.ptr = 0;
    }

    DescriptorHeapAllocation(IDescriptorAllocator&          _allocator,
                             ID3D12DescriptorHeap*          _pHeap,
                             D3D12_CPU_DESCRIPTOR_HANDLE    _cpuHandle,
                             D3D12_GPU_DESCRIPTOR_HANDLE    _gpuHandle,
                             uint32_t                       _nHandles,
                             uint16_t                       _allocationManagerId) noexcept
        : m_FirstCpuHandle      (_cpuHandle          )
        , m_FirstGpuHandle      (_gpuHandle          )
        , m_pAllocator          (&_allocator         )
        , m_pDescriptorHeap     (_pHeap              )
        , m_NumHandles          (_nHandles           )
        , m_AllocationManagerID (_allocationManagerId)
    {
        BF_ASSERT_EXPR(m_pAllocator != nullptr && m_pDescriptorHeap != nullptr);
        const uint32_t descriptorSize = m_pAllocator->GetDescriptorSize();
        BF_ASSERT(descriptorSize < std::numeric_limits<uint16_t>::max(), "Descriptor size exceeded the allowed limit");
        m_DescriptorSize = static_cast<uint16_t>(descriptorSize);
    }

    DescriptorHeapAllocation(DescriptorHeapAllocation&& _rhs) noexcept
        : m_FirstCpuHandle      (std::move(_rhs.m_FirstCpuHandle     ))
        , m_FirstGpuHandle      (std::move(_rhs.m_FirstGpuHandle     ))
        , m_pAllocator          (std::move(_rhs.m_pAllocator         ))
        , m_pDescriptorHeap     (std::move(_rhs.m_pDescriptorHeap    ))
        , m_NumHandles          (std::move(_rhs.m_NumHandles         ))
        , m_AllocationManagerID (std::move(_rhs.m_AllocationManagerID))
        , m_DescriptorSize      (std::move(_rhs.m_DescriptorSize     ))
    {
        _rhs.Reset();
    }

    DescriptorHeapAllocation& operator=(DescriptorHeapAllocation&& _rhs) noexcept
    {
        m_FirstCpuHandle      = std::move(_rhs.m_FirstCpuHandle     );
        m_FirstGpuHandle      = std::move(_rhs.m_FirstGpuHandle     );
        m_pAllocator          = std::move(_rhs.m_pAllocator         );
        m_pDescriptorHeap     = std::move(_rhs.m_pDescriptorHeap    );
        m_NumHandles          = std::move(_rhs.m_NumHandles         );
        m_AllocationManagerID = std::move(_rhs.m_AllocationManagerID);
        m_DescriptorSize      = std::move(_rhs.m_DescriptorSize     );

        _rhs.Reset();

        return *this;
    }

    DescriptorHeapAllocation           (const DescriptorHeapAllocation&) = delete;
    DescriptorHeapAllocation& operator=(const DescriptorHeapAllocation&) = delete;

    ~DescriptorHeapAllocation()
    {
        if(!IsNull() && m_pAllocator)
            m_pAllocator->Free(std::move(*this));

        BF_ASSERT(IsNull(), "Non-null descriptor is being destroyed");
    }

    void Reset()
    {
        m_FirstCpuHandle.ptr  = 0;
        m_FirstGpuHandle.ptr  = 0;
        m_pAllocator          = nullptr;
        m_pDescriptorHeap     = nullptr;
        m_NumHandles          = 0;
        m_AllocationManagerID = InvalidAllocationMgrId;
        m_DescriptorSize      = 0;
    }

    D3D12_CPU_DESCRIPTOR_HANDLE GetCpuHandle(uint32_t _offset = 0) const
    {
        BF_ASSERT_EXPR(_offset <= m_NumHandles);

        D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = m_FirstCpuHandle;
        cpuHandle.ptr += static_cast<size_t>(m_DescriptorSize) * static_cast<size_t>(_offset);

        return cpuHandle;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE GetGpuHandle(uint32_t _offset = 0) const
    {
        BF_ASSERT_EXPR(_offset <= m_NumHandles);

        D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = m_FirstGpuHandle;
        gpuHandle.ptr += static_cast<size_t>(m_DescriptorSize) * static_cast<size_t>(_offset);

        return gpuHandle;
    }

    template<typename HandleType>
    HandleType GetHandle(uint32_t _offset = 0) const;

    ID3D12DescriptorHeap* GetDescriptorHeap() const { return m_pDescriptorHeap; }

    size_t   GetNumHandles()          const { return m_NumHandles;              }
    bool     IsNull()                 const { return m_FirstCpuHandle.ptr == 0; }
    bool     IsShaderVisible()        const { return m_FirstGpuHandle.ptr != 0; }
    size_t   GetAllocationManagerId() const { return m_AllocationManagerID;     }
    uint32_t GetDescriptorSize()      const { return m_DescriptorSize;          }

private:
    D3D12_CPU_DESCRIPTOR_HANDLE m_FirstCpuHandle = {0};
    D3D12_GPU_DESCRIPTOR_HANDLE m_FirstGpuHandle = {0};

    // Pointer to the descriptor heap allocator that created this allocation
    IDescriptorAllocator* m_pAllocator      = nullptr;
    // Pointer to the D3D12 descriptor heap that contains descriptors in this allocation
    ID3D12DescriptorHeap* m_pDescriptorHeap = nullptr;

    uint32_t m_NumHandles = 0;

    static constexpr uint16_t InvalidAllocationMgrId = 0xFFFF;
    uint16_t m_AllocationManagerID = InvalidAllocationMgrId;
    uint16_t m_DescriptorSize = 0;
};

template<>
inline D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeapAllocation::GetHandle<D3D12_CPU_DESCRIPTOR_HANDLE>(uint32_t _offset) const
{
    return GetCpuHandle(_offset);
}

template <>
inline D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeapAllocation::GetHandle<D3D12_GPU_DESCRIPTOR_HANDLE>(uint32_t _offset) const
{
    return GetGpuHandle(_offset);
}

class DescriptorHeapAllocationManager
{
public:
    DescriptorHeapAllocationManager(Device&                           _device,
                                    IDescriptorAllocator&             _parentAllocator,
                                    size_t                            _thisManagerId,
                                    const D3D12_DESCRIPTOR_HEAP_DESC& _heapDesc);

    DescriptorHeapAllocationManager(Device&                 _device,
                                    IDescriptorAllocator&   _parentAllocator,
                                    size_t                  _thisManagerId,
                                    ID3D12DescriptorHeap*   _pD3D12DescriptorHeap,
                                    uint32_t                _firstDescriptor,
                                    uint32_t                _numDescriptors);

    DescriptorHeapAllocationManager(DescriptorHeapAllocationManager&& _rhs) noexcept
        : m_ParentAllocator             (_rhs.m_ParentAllocator               )
        , m_Device                      (_rhs.m_Device                        )
        , m_ThisManagerId               (_rhs.m_ThisManagerId                 )
        , m_HeapDesc                    (_rhs.m_HeapDesc                      )
        , m_DescriptorSize              (_rhs.m_DescriptorSize                )
        , m_NumDescriptorsInAllocation  (_rhs.m_NumDescriptorsInAllocation    )
        , m_FreeBlockManager            (std::move(_rhs.m_FreeBlockManager    ))
        , m_pd3d12DescriptorHeap        (std::move(_rhs.m_pd3d12DescriptorHeap))
        , m_FirstCPUHandle              (_rhs.m_FirstCPUHandle                )
        , m_FirstGPUHandle              (_rhs.m_FirstGPUHandle                )
        , m_MaxAllocatedSize            (_rhs.m_MaxAllocatedSize              )
    {
        _rhs.m_NumDescriptorsInAllocation = 0;
        _rhs.m_ThisManagerId              = ~size_t{0};
        _rhs.m_FirstCPUHandle.ptr         = 0;
        _rhs.m_FirstGPUHandle.ptr         = 0;
        _rhs.m_MaxAllocatedSize           = 0;
    }

    DescriptorHeapAllocationManager& operator = (      DescriptorHeapAllocationManager&&) = delete;
    DescriptorHeapAllocationManager             (const DescriptorHeapAllocationManager& ) = delete;
    DescriptorHeapAllocationManager& operator = (const DescriptorHeapAllocationManager& ) = delete;

    ~DescriptorHeapAllocationManager();

    DescriptorHeapAllocation    Allocate(uint32_t _count);
    void                        FreeAllocation(DescriptorHeapAllocation&& _allocation);

    size_t   GetNumAvailableDescriptors() const { return m_FreeBlockManager.GetFreeSize(); }
    uint32_t GetMaxDescriptors()          const { return m_NumDescriptorsInAllocation;     }
    size_t   GetMaxAllocatedSize()        const { return m_MaxAllocatedSize;               }


private:
    IDescriptorAllocator& m_ParentAllocator;
    Device&               m_Device;

    size_t m_ThisManagerId = static_cast<size_t>(-1);

    const D3D12_DESCRIPTOR_HEAP_DESC m_HeapDesc;

    const uint32_t m_DescriptorSize = 0;

    uint32_t m_NumDescriptorsInAllocation = 0;

    std::mutex                     m_FreeBlockManagerMutex;
    VariableSizeAllocationsManager m_FreeBlockManager;

    Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_pd3d12DescriptorHeap;

    D3D12_CPU_DESCRIPTOR_HANDLE m_FirstCPUHandle = {0};
    D3D12_GPU_DESCRIPTOR_HANDLE m_FirstGPUHandle = {0};

    size_t m_MaxAllocatedSize = 0;
};


class CPUDescriptorHeap final : public IDescriptorAllocator
{
public:
    CPUDescriptorHeap(Device&                     _device,
                      uint32_t                    _numDescriptorsInHeap,
                      D3D12_DESCRIPTOR_HEAP_TYPE  _type,
                      D3D12_DESCRIPTOR_HEAP_FLAGS _flags);

    CPUDescriptorHeap             (const CPUDescriptorHeap&) = delete;
    CPUDescriptorHeap             (CPUDescriptorHeap&&)      = delete;
    CPUDescriptorHeap& operator = (const CPUDescriptorHeap&) = delete;
    CPUDescriptorHeap& operator = (CPUDescriptorHeap&&)      = delete;

    ~CPUDescriptorHeap();

    virtual DescriptorHeapAllocation Allocate(uint32_t _count) override;
    virtual void                     Free(DescriptorHeapAllocation &&_allocation) override;
    virtual uint32_t                 GetDescriptorSize() const override { return m_DescriptorSize; }

private:
    void FreeAllocation(DescriptorHeapAllocation&& _allocation);

private:
    // Diligent uses custom allocators, i don't think we need that yet.
    // IMemoryAllocator& m_MemAllocator;
    Device&           m_Device;

    std::mutex                                     m_HeapPoolMutex;
    eastl::vector<DescriptorHeapAllocationManager> m_HeapPool;
    eastl::unordered_set<size_t>                   m_AvailableHeaps;

    D3D12_DESCRIPTOR_HEAP_DESC m_HeapDesc;
    const uint32_t             m_DescriptorSize = 0;

    uint32_t m_MaxSize     = 0;
    uint32_t m_CurrentSize = 0;
};

class GPUDescriptorHeap final : public IDescriptorAllocator
{
public:
    GPUDescriptorHeap(Device&                     _device,
                      uint32_t                    _numDescriptorsInHeap,
                      D3D12_DESCRIPTOR_HEAP_TYPE  _type,
                      D3D12_DESCRIPTOR_HEAP_FLAGS _flags);

    GPUDescriptorHeap             (const GPUDescriptorHeap& ) = delete;
    GPUDescriptorHeap& operator = (const GPUDescriptorHeap& ) = delete;
    GPUDescriptorHeap             (      GPUDescriptorHeap&&) = delete;
    GPUDescriptorHeap& operator = (      GPUDescriptorHeap&&) = delete;

    ~GPUDescriptorHeap();

    virtual DescriptorHeapAllocation Allocate(uint32_t _count) override
    {
        return m_HeapAllocationManager.Allocate(_count);
    }

    virtual uint32_t                 GetDescriptorSize() const override { return m_DescriptorSize; }
    virtual void                     Free(DescriptorHeapAllocation &&_allocation) override;

    const D3D12_DESCRIPTOR_HEAP_DESC& GetHeapDesc()             const { return m_HeapDesc; }
    uint32_t                          GetMaxStaticDescriptors() const { return m_HeapAllocationManager.GetMaxDescriptors(); }

private:
    Device& m_Device;

    const D3D12_DESCRIPTOR_HEAP_DESC m_HeapDesc;
    ComPtr<ID3D12DescriptorHeap>     m_pd3d12DescriptorHeap;

    const uint32_t m_DescriptorSize;

    std::mutex                      m_AllocationMutex;
    DescriptorHeapAllocationManager m_HeapAllocationManager;
};
} // namespace Blainn
