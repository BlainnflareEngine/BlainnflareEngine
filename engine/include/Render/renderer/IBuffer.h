//
// Created by WhoLeb on 23-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once
#include <cstdint>

#include "IBufferView.h"
#include "IDeviceObject.h"
#include "Render/GraphicsTypes.h"

namespace Blainn
{
enum BUFFER_MODE : uint8_t
{
    BUFFER_MODE_UNDEFINED = 0,
    /// Formatted buffer. Access to the buffer will use format conversion operations.
    /// In this mode, ElementByteStride member of BufferDesc defines the buffer element size.
    /// Buffer views can use different formats, but the format size must match ElementByteStride.
    BUFFER_MODE_FORMATTED,

    /// Structured buffer.
    /// In this mode, ElementByteStride member of BufferDesc defines the structure stride.
    BUFFER_MODE_STRUCTURED,

    /// Raw buffer.
    /// In this mode, the buffer is accessed as raw bytes. Formatted views of a raw
    /// buffer can also be created similar to formatted buffer. If formatted views
    /// are to be created, the ElementByteStride member of BufferDesc must specify the
    /// size of the format.
    BUFFER_MODE_RAW,

    BUFFER_MODE_NUM_MODES
};
DEFINE_FLAG_ENUM_OPERATORS(BUFFER_MODE);

enum MISC_BUFFER_FLAGS : uint8_t
{
    MISC_BUFFER_FLAG_NONE = 0,

    /// For a sparse buffer, allow binding the same memory region in different buffer ranges
    /// or in different sparse buffers.
    MISC_BUFFER_FLAG_SPARSE_ALIASING = 1
};
DEFINE_FLAG_ENUM_OPERATORS(MISC_BUFFER_FLAGS);

struct BufferDesc : DeviceObjectAttribs
{
    /// Size of the buffer, in bytes. For a uniform buffer, this must be a multiple of 16.
    uint64_t          Size                 = 0;

    BIND_FLAGS        BindFlags            = BIND_NONE;

    USAGE             Usage                = USAGE_DEFAULT;

    CPU_ACCESS_FLAGS  CPUAccessFlags       = CPU_ACCESS_NONE;

    BUFFER_MODE       Mode                 = BUFFER_MODE_UNDEFINED;

    MISC_BUFFER_FLAGS MiscFlags            = MISC_BUFFER_FLAG_NONE;

    /// Buffer element stride, in bytes.

    /// For a structured buffer (BufferDesc::Mode equals BUFFER_MODE_STRUCTURED) this member
    /// defines the size of each buffer element. For a formatted buffer
    /// (BufferDesc::Mode equals BUFFER_MODE_FORMATTED) and optionally for a raw buffer
    /// (BUFFER_MODE_RAW), this member defines the size of the format that will be used for views
    /// created for this buffer.
    uint32_t          ElementByteStride    = 0;

    /// Defines which immediate contexts are allowed to execute commands that use this buffer.

    /// When ImmediateContextMask contains a bit at position n, the buffer may be
    /// used in the immediate context with index n directly (see DeviceContextDesc::ContextId).
    /// It may also be used in a command list recorded by a deferred context that will be executed
    /// through that immediate context.
    ///
    /// \remarks    Only specify these bits that will indicate those immediate contexts where the buffer
    ///             will actually be used. Do not set unnecessary bits as this will result in extra overhead.
    uint64_t          ImmediateContextMask = 1;

        constexpr BufferDesc() noexcept {}

    constexpr BufferDesc(const char*      _name,
                         uint64_t         _size,
                         BIND_FLAGS       _bindFlags,
                         USAGE            _usage                = BufferDesc{}.Usage,
                         CPU_ACCESS_FLAGS _CPUAccessFlags       = BufferDesc{}.CPUAccessFlags,
                         BUFFER_MODE      _mode                 = BufferDesc{}.Mode,
                         uint32_t         _elementByteStride    = BufferDesc{}.ElementByteStride,
                         uint64_t         _immediateContextMask = BufferDesc{}.ImmediateContextMask) noexcept :
        DeviceObjectAttribs  {_name             },
        Size                 {_size             },
        BindFlags            {_bindFlags        },
        Usage                {_usage            },
        CPUAccessFlags       {_CPUAccessFlags   },
        Mode                 {_mode             },
        ElementByteStride    {_elementByteStride},
        ImmediateContextMask {_immediateContextMask}
    {
    }

    constexpr bool operator == (const BufferDesc& _rhs)const
    {
                // Name is primarily used for debug purposes and does not affect the state.
                // It is ignored in comparison operation.
        return  // strcmp(Name, RHS.Name) == 0          &&
                Size                 == _rhs.Size              &&
                BindFlags            == _rhs.BindFlags         &&
                Usage                == _rhs.Usage             &&
                CPUAccessFlags       == _rhs.CPUAccessFlags    &&
                Mode                 == _rhs.Mode              &&
                ElementByteStride    == _rhs.ElementByteStride &&
                ImmediateContextMask == _rhs.ImmediateContextMask;
    }
};

struct BufferData
{
    const void* pData = nullptr;

    uint64_t Size = 0;

    /// Defines which device context will be used to initialize the buffer.

    /// The buffer will be in write state after the initialization.
    /// If an application uses the buffer in another context afterwards, it
    /// must synchronize the access to the buffer using fence.
    /// When null is provided, the first context enabled by ImmediateContextMask
    /// will be used.
    struct IDeviceContext* pContext = nullptr;

    constexpr BufferData() noexcept {}

    constexpr BufferData(const void*     _pData,
                         uint64_t        _dataSize,
                         IDeviceContext* _pContext) noexcept
        : pData(_pData)
        , Size(_dataSize)
        , pContext(_pContext)
    {}
};

struct SparseBufferProperties
{
    uint64_t AddressSpaceSize = 0;

    /// The size of the sparse memory block.

    /// \note Offset in the buffer, memory offset and memory size that are used in sparse resource
    ///       binding command, must be multiples of the block size.
    ///       In Direct3D11 and Direct3D12, the block size is always 64Kb.
    ///       In Vulkan, the block size is not documented, but is usually also 64Kb.
    uint32_t BlockSize        = 0;
};

struct IBuffer : public IDeviceObject
{
    virtual const BufferDesc& GetDesc() const = 0;

    /// Creates a new buffer view

    /// \param [in] _viewDesc - View description. See Diligent::BufferViewDesc for details.
    /// \param [out] _ppView - Address of the memory location where the pointer to the view interface will be written to.
    ///
    /// \remarks To create a view addressing the entire buffer, set only BufferViewDesc::ViewType member
    ///          of the ViewDesc structure and leave all other members in their default values.\n
    ///          Buffer view will contain strong reference to the buffer, so the buffer will not be destroyed
    ///          until all views are released.\n
    virtual void CreateView(const BufferViewDesc& _viewDesc,
                                  IBufferView**   _ppView) = 0;

    /// \param [in] _viewType - Type of the requested view. See Diligent::BUFFER_VIEW_TYPE.
    /// \return Pointer to the interface
    ///
    /// \remarks Default views are only created for structured and raw buffers. As for formatted buffers
    ///          the view format is unknown at buffer initialization time, no default views are created.
    virtual IBufferView* GetDefaultView(BUFFER_VIEW_TYPE _viewType) = 0;

    /// Returns native buffer handle specific to the underlying graphics API

    /// \return A pointer to `ID3D11Resource` interface, for D3D11 implementation\n
    ///         A pointer to `ID3D12Resource` interface, for D3D12 implementation\n
    ///         `VkBuffer` handle, for Vulkan implementation\n
    ///         GL buffer name, for OpenGL implementation\n
    ///         `MtlBuffer`, for Metal implementation\n
    ///         `WGPUBuffer`, for WGPU implementation\n
    virtual uint64_t GetNativeHandle() = 0;

    /// Sets the buffer usage state.

    /// \note This method does not perform state transition, but
    ///       resets the internal buffer state to the given value.
    ///       This method should be used after the application finished
    ///       manually managing the buffer state and wants to hand over
    ///       state management back to the engine.
    virtual void SetState(RESOURCE_STATE _state) = 0;

    /// Returns the internal buffer state
    virtual RESOURCE_STATE GetState() const = 0;

    /// Returns the buffer memory properties, see Diligent::MEMORY_PROPERTIES.

    /// The memory properties are only relevant for persistently mapped buffers.
    /// In particular, if the memory is not coherent, an application must call
    /// IBuffer::FlushMappedRange() to make writes by the CPU available to the GPU, and
    /// call IBuffer::InvalidateMappedRange() to make writes by the GPU visible to the CPU.
    virtual MEMORY_PROPERTIES GetMemoryProperties() const = 0;

    /// Flushes the specified range of non-coherent memory from the host cache to make
    /// it available to the GPU.

    /// \param [in] _startOffset - Offset, in bytes, from the beginning of the buffer to
    ///                           the start of the memory range to flush.
    /// \param [in] _size        - Size, in bytes, of the memory range to flush.
    ///
    /// This method should only be used for persistently-mapped buffers that do not
    /// report MEMORY_PROPERTY_HOST_COHERENT property. After an application modifies
    /// a mapped memory range on the CPU, it must flush the range to make it available
    /// to the GPU.
    ///
    /// \note   This method must never be used for USAGE_DYNAMIC buffers.
    ///
    /// When a mapped buffer is unmapped it is automatically flushed by
    /// the engine if necessary.
    virtual void FlushMappedRange(uint64_t _startOffset,
                                  uint64_t _size) = 0;

    /// Invalidates the specified range of non-coherent memory modified by the GPU to make
    /// it visible to the CPU.

    /// \param [in] _startOffset - Offset, in bytes, from the beginning of the buffer to
    ///                           the start of the memory range to invalidate.
    /// \param [in] _size        - Size, in bytes, of the memory range to invalidate.
    ///
    /// This method should only be used for persistently-mapped buffers that do not
    /// report MEMORY_PROPERTY_HOST_COHERENT property. After an application modifies
    /// a mapped memory range on the GPU, it must invalidate the range to make it visible
    /// to the CPU.
    ///
    /// \note   This method must never be used for USAGE_DYNAMIC buffers.
    ///
    /// When a mapped buffer is unmapped, it is automatically invalidated by
    /// the engine if necessary.
    virtual void InvalidateMappedRange(uint64_t _startOffset,
                                       uint64_t _size) = 0;

    virtual SparseBufferProperties GetSparseProperties() const = 0;
};

}