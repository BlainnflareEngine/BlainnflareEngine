#pragma once

#include "EASTL/deque.h"
#include "helpers.h"
#include "VariableSizeAllocationsManager.h"

namespace Blainn
{
class VariableSizeGPUAllocationsManager : public VariableSizeAllocationsManager
{
    struct StaleAllocationAttribs
    {
        OffsetType Offset;
        OffsetType Size;
        uint64_t FenceValue;
        
        //NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        StaleAllocationAttribs(OffsetType _offset, OffsetType _size, uint64_t _fenceValue)
            : Offset(_offset)
            , Size(_size)
            , FenceValue(_fenceValue)
        {}
    };

public:
    VariableSizeGPUAllocationsManager(OffsetType _maxSize)
        : VariableSizeAllocationsManager(_maxSize)
        , m_StaleAllocations(0, StaleAllocationAttribs(0, 0, 0))
    {}

    ~VariableSizeGPUAllocationsManager()
    {
        BF_ASSERT(m_StaleAllocations.empty(), "Not all stale allocations were released");
        BF_ASSERT(m_StaleAllocationSize == 0, "Not all stale allocations were released");
    }

    VariableSizeGPUAllocationsManager(VariableSizeGPUAllocationsManager&& rhs) noexcept
        : VariableSizeAllocationsManager(std::move(rhs))
        , m_StaleAllocations(std::move(rhs.m_StaleAllocations))
        , m_StaleAllocationSize(rhs.m_StaleAllocationSize)
    {
        rhs.m_StaleAllocationSize = 0;
    }

    VariableSizeGPUAllocationsManager& operator = (      VariableSizeAllocationsManager&&) = delete;
    VariableSizeGPUAllocationsManager             (const VariableSizeAllocationsManager& ) = delete;
    VariableSizeGPUAllocationsManager& operator = (const VariableSizeAllocationsManager& ) = delete;

    void Free(VariableSizeAllocationsManager::Allocation&& _allocation, uint64_t _fenceValue)
    {
        Free(_allocation.UnalignedOffset, _allocation.Size, _fenceValue);
        _allocation = VariableSizeAllocationsManager::Allocation{};
    }

    //NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    void Free(OffsetType _offset, OffsetType _size, uint64_t _fenceValue)
    {
        m_StaleAllocations.emplace_back(_offset, _size, _fenceValue);
        m_StaleAllocationSize += _size;
    }

    void ReleaseStaleAllocations(uint64_t _lastCompletedFenceValue)
    {
        while(!m_StaleAllocations.empty() && m_StaleAllocations.front().FenceValue <= _lastCompletedFenceValue)
        {
            StaleAllocationAttribs& oldestAllocation = m_StaleAllocations.front();
            VariableSizeAllocationsManager::Free(oldestAllocation.Offset, oldestAllocation.Size);
            m_StaleAllocationSize -= oldestAllocation.Size;
            m_StaleAllocations.pop_front();
        }
    }

private:
    eastl::deque<StaleAllocationAttribs> m_StaleAllocations;
    size_t                               m_StaleAllocationSize = 0;
};
} // namespace Blainn