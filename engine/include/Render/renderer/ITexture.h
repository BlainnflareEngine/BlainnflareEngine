//
// Created by WhoLeb on 21-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once

#include <cstdint>
#include <Render/GraphicsTypes.h>

#include "IDeviceObject.h"
#include "ITextureView.h"

namespace Blainn
{
enum MISC_TEXTURE_FLAGS : uint8_t
{
    /// No special flags are set
    MISC_TEXTURE_FLAG_NONE             = 0u,

    /// Allow automatic mipmap generation with IDeviceContext::GenerateMips()

    /// \note A texture must be created with BIND_RENDER_TARGET bind flag.
    MISC_TEXTURE_FLAG_GENERATE_MIPS    = 1u << 0,

    /// The texture will be used as a transient framebuffer attachment.

    /// Memoryless textures may only be used within a render pass in a framebuffer;
    /// the corresponding subpass load operation must be CLEAR or DISCARD, and the
    /// subpass store operation must be DISCARD.
    MISC_TEXTURE_FLAG_MEMORYLESS      = 1u << 1,

    /// For sparse textures, allow binding the same memory range in different texture
    /// regions or in different sparse textures.
    MISC_TEXTURE_FLAG_SPARSE_ALIASING = 1u << 2,

    /// The texture will be used as an intermediate render target for rendering with
    /// texture-based variable rate shading.
    /// Requires SHADING_RATE_CAP_FLAG_SUBSAMPLED_RENDER_TARGET capability.
    ///
    /// \note  Copy operations are not supported for subsampled textures.
    MISC_TEXTURE_FLAG_SUBSAMPLED      = 1u << 3
};

struct TextureDesc : public DeviceObjectAttribs
{
    RESOURCE_DIMENSION Type   = RESOURCE_DIM_UNDEFINED;
    uint32_t           Width  = 0;
    uint32_t           Height = 0;
    union
    {
        uint32_t ArraySize = 1;
        uint32_t Depth        ;
    };

    TEXTURE_FORMAT Format   = TEX_FORMAT_UNKNOWN;

    /// Number of mip levels. Multisampled textures can only have 1 mip level
    /// Specify 0 to create the full mip chain
    uint32_t MipLevels = 1;

    /// The number of samples.
    /// Only 2D textures or 2D texture arrays can be multisampled.
    uint32_t SampleCount = 1;

    /// Bind flags, see BIND_FLAGS for details.
    /// Use IRenderDevice::GetTextureFormatInfoExt() to check which bind flags are supported.
    BIND_FLAGS BindFlags = BIND_NONE;

    USAGE              Usage          = USAGE_DEFAULT;
    CPU_ACCESS_FLAGS   CPUAccessFlags = CPU_ACCESS_NONE;
    MISC_TEXTURE_FLAGS MiscFlags      = MISC_TEXTURE_FLAG_NONE;

    OptimizedClearValue ClearValue;

    /// Defines which immediate contexts are allowed to execute commands that use this texture.

    /// When `ImmediateContextMask` contains a bit at position n, the texture may be
    /// used in the immediate context with index n directly (see DeviceContextDesc::ContextId).
    /// It may also be used in a command list recorded by a deferred context that will be executed
    /// through that immediate context.
    ///
    /// \remarks    Only specify these bits that will indicate those immediate contexts where the texture
    ///             will actually be used. Do not set unnecessary bits as this will result in extra overhead.
    uint64_t ImmediateContextMask = 1;

    constexpr TextureDesc() {}

    constexpr TextureDesc(const char*          _name,
                           RESOURCE_DIMENSION  _type,
                           uint32_t            _width,
                           uint32_t            _height,
                           uint32_t            _arraySizeOrDepth,
                           TEXTURE_FORMAT      _format,
                           uint32_t            _mipLevels            = TextureDesc{}.MipLevels,
                           uint32_t            _sampleCount          = TextureDesc{}.SampleCount,
                           USAGE               _usage                = TextureDesc{}.Usage,
                           BIND_FLAGS          _bindFlags            = TextureDesc{}.BindFlags,
                           CPU_ACCESS_FLAGS    _cpuAccessFlags       = TextureDesc{}.CPUAccessFlags,
                           MISC_TEXTURE_FLAGS  _miscFlags            = TextureDesc{}.MiscFlags,
                           OptimizedClearValue _clearValue           = TextureDesc{}.ClearValue,
                           uint64_t            _immediateContextMask = TextureDesc{}.ImmediateContextMask) noexcept
         : DeviceObjectAttribs  {_name            }
         , Type                 {_type            }
         , Width                {_width           }
         , Height               {_height          }
         , ArraySize            {_arraySizeOrDepth}
         , Format               {_format          }
         , MipLevels            {_mipLevels       }
         , SampleCount          {_sampleCount     }
         , BindFlags            {_bindFlags       }
         , Usage                {_usage           }
         , CPUAccessFlags       {_cpuAccessFlags  }
         , MiscFlags            {_miscFlags       }
         , ClearValue           {_clearValue      }
         , ImmediateContextMask {_immediateContextMask}
    {}

    constexpr uint32_t ArraySizeOrDepth() const { return ArraySize; }

   /// Tests if two texture descriptions are equal.

    /// \param [in] RHS - reference to the structure to compare with.
    ///
    /// \return     true if all members of the two structures *except for the Name* are equal,
    ///             and false otherwise.
    ///
    /// \note   The operator ignores the Name field as it is used for debug purposes and
    ///         doesn't affect the texture properties.
    constexpr bool operator ==(const TextureDesc& RHS)const
    {
                // Name is primarily used for debug purposes and does not affect the state.
                // It is ignored in comparison operation.
        return  // strcmp(Name, RHS.Name) == 0          &&
                Type                 == RHS.Type           &&
                Width                == RHS.Width          &&
                Height               == RHS.Height         &&
                ArraySizeOrDepth()   == RHS.ArraySizeOrDepth() &&
                Format               == RHS.Format         &&
                MipLevels            == RHS.MipLevels      &&
                SampleCount          == RHS.SampleCount    &&
                Usage                == RHS.Usage          &&
                BindFlags            == RHS.BindFlags      &&
                CPUAccessFlags       == RHS.CPUAccessFlags &&
                MiscFlags            == RHS.MiscFlags      &&
                ClearValue           == RHS.ClearValue     &&
                ImmediateContextMask == RHS.ImmediateContextMask;
    }

    constexpr bool IsArray() const
    {
        return Type == RESOURCE_DIM_TEX_1D_ARRAY ||
               Type == RESOURCE_DIM_TEX_2D_ARRAY ||
               Type == RESOURCE_DIM_TEX_CUBE     ||
               Type == RESOURCE_DIM_TEX_CUBE_ARRAY;
    }

    constexpr bool Is1D() const
    {
        return Type == RESOURCE_DIM_TEX_1D      ||
               Type == RESOURCE_DIM_TEX_1D_ARRAY;
    }

    constexpr bool Is2D() const
    {
        return Type == RESOURCE_DIM_TEX_2D       ||
               Type == RESOURCE_DIM_TEX_2D_ARRAY ||
               Type == RESOURCE_DIM_TEX_CUBE     ||
               Type == RESOURCE_DIM_TEX_CUBE_ARRAY;
    }

    constexpr bool Is3D() const
    {
        return Type == RESOURCE_DIM_TEX_3D;
    }

    constexpr bool IsCube() const
    {
        return Type == RESOURCE_DIM_TEX_CUBE     ||
               Type == RESOURCE_DIM_TEX_CUBE_ARRAY;
    }

    constexpr uint32_t GetArraySize() const
    {
        return IsArray() ? ArraySize : 1u;
    }

    constexpr uint32_t GetWidth() const
    {
        return Width;
    }

    constexpr uint32_t GetHeight() const
    {
        return Is1D() ? 1u : Height;
    }

    constexpr uint32_t GetDepth() const
    {
        return Is3D() ? Depth : 1u;
    } 
};

struct TextureSubResData
{
    const  void*    pData      = nullptr;
    struct IBuffer* pSrcBuffer = nullptr;

    uint64_t SrcOffset   = 0;
    uint64_t Stride      = 0;
    uint64_t DepthStride = 0;

    constexpr TextureSubResData() noexcept {}

    /// Initializes the structure members to perform copy from the CPU memory
    constexpr TextureSubResData(const void* _pData, uint64_t _stride, uint64_t _depthStride = 0) noexcept
        : pData       {_pData      }
        , pSrcBuffer  {nullptr     }
        , SrcOffset   {0           }
        , Stride      {_stride     }
        , DepthStride {_depthStride}
    {}

    /// Initializes the structure members to perform copy from the GPU buffer
    constexpr TextureSubResData(IBuffer* _pBuffer, uint64_t _srcOffset, uint64_t _stride, uint64_t _depthStride = 0) noexcept
        : pData       {nullptr     }
        , pSrcBuffer  {_pBuffer    }
        , SrcOffset   {_srcOffset  }
        , Stride      {_stride     }
        , DepthStride {_depthStride}
    {}
};

struct TextureData
{
    TextureSubResData*     pSubresources   = nullptr;
    uint32_t               NumSubresources = 0;
    struct IDeviceContext* pDeviceContext  = nullptr;

    TextureData() noexcept {}

    constexpr TextureData(TextureSubResData* _pSubresources,
                          uint32_t           _numSubresources,
                          IDeviceContext*    _pContext = nullptr) noexcept
        : pSubresources   {_pSubresources  }
        , NumSubresources {_numSubresources}
        , pDeviceContext  {_pContext       }
    {}
};

struct MappedTextureSubresource
{
    /// Pointer to the mapped subresource data in CPU memory.
    void*  pData       = nullptr;

    /// Row stride in bytes.
    uint64_t Stride      = 0;

    /// Depth slice stride in bytes.
    uint64_t DepthStride = 0;

    constexpr MappedTextureSubresource() noexcept {}

    constexpr MappedTextureSubresource(void*    _pData,
                                       uint64_t _stride,
                                       uint64_t _depthStride = 0) noexcept
        : pData       {_pData      }
        , Stride      {_stride     }
        , DepthStride {_depthStride}
    {}
};

struct SparseTextureProperties
{
    /// The size of the texture's virtual address space.
    uint64_t AddressSpaceSize = 0;

    /// Specifies where to bind the mip tail memory.
    /// Reserved for internal use.
    uint64_t MipTailOffset    = 0;

    /// Specifies how to calculate the mip tail offset for 2D array texture.
    /// Reserved for internal use.
    uint64_t MipTailStride    = 0;

    /// Specifies the mip tail size in bytes.

    /// \note Single mip tail for a 2D array may exceed the 32-bit limit.
    uint64_t MipTailSize      = 0;

    /// The first mip level in the mip tail that is packed as a whole into one
    /// or multiple memory blocks.
    uint32_t FirstMipInTail   = ~0u;

    /// Specifies the dimension of a tile packed into a single memory block.
    uint32_t TileSize[3]      = {};

    /// Size of the sparse memory block, in bytes.

    /// The offset in the packed mip tail, memory offset and memory size that are used in sparse
    /// memory binding command must be multiples of the block size.
    ///
    /// If the SPARSE_TEXTURE_FLAG_NONSTANDARD_BLOCK_SIZE flag is not set in the `Flags` member,
    /// the block size is equal to SparseResourceProperties::StandardBlockSize.
    uint32_t BlockSize = 0;

    /// Flags that describe additional packing modes.
    SPARSE_TEXTURE_FLAGS Flags = SPARSE_TEXTURE_FLAG_NONE;
};

struct ITexture : IDeviceObject
{
    /// Creates a new texture view

    /// \param [in] _viewDesc - View description. See TextureViewDesc for details.
    /// \param [out] _ppView - Address of the memory location where the pointer to the view interface will be written to.
    ///
    /// To create a shader resource view addressing the entire texture, set only TextureViewDesc::ViewType
    /// member of the ViewDesc parameter to TEXTURE_VIEW_SHADER_RESOURCE and leave all other
    /// members in their default values. Using the same method, you can create render target or depth stencil
    /// view addressing the largest mip level.
    ///
    /// If texture view format is TEX_FORMAT_UNKNOWN, the view format will match the texture format.
    ///
    /// If texture view type is TEXTURE_VIEW_UNDEFINED, the type will match the texture type.
    /// If the number of mip levels is 0, and the view type is shader resource, the view will address all mip levels.
    ///
    /// For other view types it will address one mip level.
    ///
    /// If the number of slices is 0, all slices from FirstArraySlice or FirstDepthSlice will be referenced by the view.
    /// For non-array textures, the only allowed values for the number of slices are 0 and 1.
    ///
    /// Texture view will contain strong reference to the texture, so the texture will not be destroyed
    /// until all views are released.
    virtual void CreateView(const TextureViewDesc& _viewDesc,
                                  ITextureView**   _ppView) = 0;

    virtual ITextureView* GetDefaultView(TEXTURE_VIEW_TYPE _viewType) = 0;

    /// Returns native texture handle specific to the underlying graphics API

    /// \return A pointer to `ID3D11Resource` interface, for D3D11 implementation\n
    ///         A pointer to `ID3D12Resource` interface, for D3D12 implementation\n
    ///         `VkImage` handle, for Vulkan implementation\n
    ///         GL texture name, for OpenGL implementation\n
    ///         `MtlTexture`, for Metal implementation\n
    ///         `WGPUTexture` for WebGPU implementation
    virtual uint64_t GetNativeHandle() = 0;

    virtual void SetState(RESOURCE_STATE _state) = 0;
    virtual RESOURCE_STATE GetState() const = 0;

    virtual const SparseTextureProperties& GetSparseProperties() const = 0;
};

}