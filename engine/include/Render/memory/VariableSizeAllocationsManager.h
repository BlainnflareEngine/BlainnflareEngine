#pragma once

#include "align.h"
#include "EASTL/map.h"
#include "helpers.h"
#include <cstddef>
#include <oaidl.h>

namespace Blainn
{
class VariableSizeAllocationsManager
{
public:
    using OffsetType = size_t;

private:
    struct FreeBlockInfo;

    using TFreeBlocksByOffsetMap = 
        eastl::map<OffsetType, FreeBlockInfo>;

    using TFreeBlocksBySizeMap = 
        eastl::multimap<OffsetType, TFreeBlocksByOffsetMap::iterator>;

    struct FreeBlockInfo
    {
        OffsetType Size;
        TFreeBlocksBySizeMap::iterator OrderBySizeIt;

        FreeBlockInfo(OffsetType _size)
        : Size(_size) {}
    };

public:
    struct CreateInfo
    {
        OffsetType MaxSize = 0;
    };

    explicit VariableSizeAllocationsManager(const CreateInfo& CI)
        : m_MaxSize(CI.MaxSize)
        , m_FreeSize(CI.MaxSize)
    {
        AddNewBlock(0, m_MaxSize);
        ResetCurrAlignment();
    }

    VariableSizeAllocationsManager(OffsetType MaxSize)
        : VariableSizeAllocationsManager(CreateInfo{MaxSize})
    {}

    ~VariableSizeAllocationsManager() {}

    VariableSizeAllocationsManager(VariableSizeAllocationsManager&& rhs) noexcept
        : m_FreeBlocksByOffset(std::move(rhs.m_FreeBlocksByOffset))
        , m_FreeBlocksBySize  (std::move(rhs.m_FreeBlocksBySize)  )
        , m_MaxSize           (rhs.m_MaxSize)
        , m_FreeSize          (rhs.m_FreeSize)
        , m_CurrAlignment     (rhs.m_CurrAlignment)
    {
        rhs.m_FreeBlocksByOffset.clear();
        rhs.m_FreeBlocksBySize.clear();
        rhs.m_MaxSize       = 0;
        rhs.m_FreeSize      = 0;
        rhs.m_CurrAlignment = 0;
    }

    VariableSizeAllocationsManager& operator = (      VariableSizeAllocationsManager&&) = delete;
    VariableSizeAllocationsManager             (const VariableSizeAllocationsManager& ) = delete;
    VariableSizeAllocationsManager& operator = (const VariableSizeAllocationsManager& ) = delete;

    struct Allocation 
    {
        Allocation() = default;
        //NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
        Allocation(OffsetType _offset, OffsetType _size)
            : UnalignedOffset(_offset)
            , Size(_size)
        {}

        static constexpr OffsetType InvalidOffset = ~OffsetType{0};
        static Allocation           InvalidAllocation()
        { return Allocation{InvalidOffset, 0}; }

        bool IsValid() const { return UnalignedOffset != InvalidOffset; }

        bool operator==(const Allocation& rhs) const noexcept 
        {
            return UnalignedOffset == rhs.UnalignedOffset && Size == rhs.Size;
        }

        OffsetType UnalignedOffset = InvalidOffset;
        OffsetType Size            = 0;
    };

    //NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    Allocation Allocate(OffsetType _size, OffsetType _alignment)
    {
        BF_ASSERT_EXPR(_size > 0);
        BF_ASSERT(Blainn::IsPowerOfTwo(_size), "alignment ({}) must be a power of 2", _alignment);

        _size = Blainn::AlignUp(_size, _alignment);
        if(m_FreeSize < _size)
            return Allocation::InvalidAllocation();

        OffsetType alignmentReserve = (_alignment > m_CurrAlignment) ? _alignment - m_CurrAlignment : 0;

        auto smallestBlockItIt = m_FreeBlocksBySize.lower_bound(_size + alignmentReserve);
        if(smallestBlockItIt == m_FreeBlocksBySize.end())
            return Allocation::InvalidAllocation(); 

        auto smallestBlockIt = smallestBlockItIt->second;
        BF_ASSERT_EXPR(_size + alignmentReserve <= smallestBlockIt->second.Size);
        BF_ASSERT_EXPR(smallestBlockIt->second.Size == smallestBlockIt->first);

        OffsetType offset = smallestBlockIt->first;
        BF_ASSERT_EXPR(offset % m_CurrAlignment == 0);
        OffsetType alignedOffset = Blainn::AlignUp(offset, _alignment);
        OffsetType adjustedSize  = _size + (alignedOffset - offset);
        BF_ASSERT_EXPR(adjustedSize <= _size + alignmentReserve);
        OffsetType newOffset = offset + adjustedSize;
        OffsetType newSize   = smallestBlockIt->second.Size - adjustedSize;
        BF_ASSERT_EXPR(smallestBlockItIt == smallestBlockIt->second.OrderBySizeIt);
        m_FreeBlocksBySize.erase(smallestBlockItIt);
        m_FreeBlocksByOffset.erase(smallestBlockIt);
        if(newSize > 0)
            AddNewBlock(newOffset, newSize);

        m_FreeSize -= adjustedSize;

        if((_size & (m_CurrAlignment - 1)) != 0)
        {
            if(Blainn::IsPowerOfTwo(_size))
            {
                BF_ASSERT_EXPR(_size >= _alignment && _size < m_CurrAlignment);
                m_CurrAlignment = _size;
            }
            else
            {
                m_CurrAlignment = std::min(m_CurrAlignment, _alignment);
            }
        }

        return Allocation{offset, adjustedSize};
    } 

    void Free(Allocation&& _allocation)
    {
        BF_ASSERT_EXPR(_allocation.IsValid());
        Free(_allocation.UnalignedOffset, _allocation.Size);
        _allocation = Allocation{};
    }

    //NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    void Free(OffsetType _offset, OffsetType _size)
    {
        BF_ASSERT_EXPR(_offset != Allocation::InvalidOffset && _offset + _size <= m_MaxSize);

        auto nextBlockIt = m_FreeBlocksByOffset.upper_bound(_offset);

        BF_ASSERT_EXPR(nextBlockIt == m_FreeBlocksByOffset.end() || _offset + _size <= nextBlockIt->first);
        auto prevBlockIt = nextBlockIt;
        if(prevBlockIt != m_FreeBlocksByOffset.begin())
        {
            --prevBlockIt;
            BF_ASSERT_EXPR(_offset >= prevBlockIt->first + prevBlockIt->second.Size);
        }
        else
        {
            prevBlockIt = m_FreeBlocksByOffset.end();
        }

        OffsetType newSize, newOffset;
        if(prevBlockIt != m_FreeBlocksByOffset.end() && _offset == prevBlockIt->first + prevBlockIt->second.Size)
        {
            newSize   = prevBlockIt->second.Size + _size;
            newOffset = prevBlockIt->first;

            if(nextBlockIt != m_FreeBlocksByOffset.end() && _offset + _size == nextBlockIt->first)
            {
                newSize += nextBlockIt->second.Size;
                m_FreeBlocksBySize.erase(prevBlockIt->second.OrderBySizeIt);
                m_FreeBlocksBySize.erase(nextBlockIt->second.OrderBySizeIt);
                ++nextBlockIt;
                m_FreeBlocksByOffset.erase(prevBlockIt, nextBlockIt);
            }
            else
            {
                m_FreeBlocksBySize.erase(prevBlockIt->second.OrderBySizeIt);
                m_FreeBlocksByOffset.erase(prevBlockIt);
            }
        }
        else if(nextBlockIt!= m_FreeBlocksByOffset.end() && _offset + _size == nextBlockIt->first)
        {
            newSize   = _size + nextBlockIt->second.Size;
            newOffset = _offset;
            m_FreeBlocksBySize.erase(nextBlockIt->second.OrderBySizeIt);
            m_FreeBlocksByOffset.erase(nextBlockIt);
        }
        else
        {
            newSize   = _size;
            newOffset = _offset;
        }

        AddNewBlock(newOffset, newSize);

        m_FreeSize += _size;
        if(IsEmpty())
        {
            BF_ASSERT_EXPR(GetNumFreeBlocks() == 1);
            ResetCurrAlignment();
        }
    }

    bool IsFull()  const { return m_FreeSize == 0; }
    bool IsEmpty() const { return m_FreeSize == m_MaxSize; }
    OffsetType GetMaxSize()  const { return m_MaxSize; }
    OffsetType GetFreeSize() const { return m_FreeSize; }
    OffsetType GetUsedSize() const { return m_MaxSize - m_FreeSize; }

    size_t GetNumFreeBlocks() const 
    {
        return m_FreeBlocksByOffset.size();
    }

    size_t GetMaxFreeBlockSize() const
    {
        return !m_FreeBlocksBySize.empty() ? m_FreeBlocksBySize.rbegin()->first : 0;
    }

    void Extend(size_t _extraSize)
    {
        size_t newBlockOffset = m_MaxSize;
        size_t newBlockSize   = _extraSize;

        if(!m_FreeBlocksByOffset.empty())
        {
            auto lastBlockIt = m_FreeBlocksByOffset.end();
            --lastBlockIt;

            const OffsetType lastBlockOffset = lastBlockIt->first;
            const OffsetType lastBlockSize   = lastBlockIt->second.Size;
            if(lastBlockOffset + lastBlockSize == m_MaxSize)
            {
                newBlockOffset = lastBlockOffset;
                newBlockSize  += lastBlockSize;
                BF_ASSERT_EXPR(lastBlockIt->second.OrderBySizeIt->first == lastBlockSize
                                && lastBlockIt->second.OrderBySizeIt->second == lastBlockIt);

                m_FreeBlocksBySize.erase(lastBlockIt->second.OrderBySizeIt);
                m_FreeBlocksByOffset.erase(lastBlockIt);
            }
        }
        
        AddNewBlock(newBlockOffset, newBlockSize);
        m_MaxSize += _extraSize;
        m_FreeSize += _extraSize;
    }

private:

    //NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
    void AddNewBlock(OffsetType _offset, OffsetType _size)
    {
        auto newBlockIt = m_FreeBlocksByOffset.emplace(_offset, _size);
        BF_ASSERT_EXPR(newBlockIt.second);
        auto orderIt    = m_FreeBlocksBySize.emplace(_size, newBlockIt.first);
        newBlockIt.first->second.OrderBySizeIt = orderIt;
    }

    void ResetCurrAlignment()
    {
        for(m_CurrAlignment = 1; m_CurrAlignment * 2 < m_MaxSize; m_CurrAlignment *= 2) {}
    }
    
private:
    TFreeBlocksByOffsetMap  m_FreeBlocksByOffset;
    TFreeBlocksBySizeMap    m_FreeBlocksBySize;

    OffsetType m_MaxSize        = 0;
    OffsetType m_FreeSize       = 0;
    OffsetType m_CurrAlignment  = 0;
};
} // namespace Blainn
