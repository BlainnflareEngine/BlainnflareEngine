//
// Created by WhoLeb on 21-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once

#include "IDeviceObject.h"
#include "Render/GraphicsTypes.h"

namespace Blainn
{
struct ISampler;

enum UAV_ACCESS_FLAG : uint8_t
{
    /// Access mode is unspecified
    UAV_ACCESS_UNSPECIFIED = 0x00,

    /// Allow read operations on the UAV
    UAV_ACCESS_FLAG_READ   = 0x01,

    /// Allow write operations on the UAV
    UAV_ACCESS_FLAG_WRITE  = 0x02,

    /// Allow read and write operations on the UAV
    UAV_ACCESS_FLAG_READ_WRITE = UAV_ACCESS_FLAG_READ | UAV_ACCESS_FLAG_WRITE,

    UAV_ACCESS_FLAG_LAST = UAV_ACCESS_FLAG_READ_WRITE
};
DEFINE_FLAG_ENUM_OPERATORS(UAV_ACCESS_FLAG);

enum TEXTURE_VIEW_FLAGS : uint8_t
{
    TEXTURE_VIEW_FLAG_NONE = 0x00,
    TEXTURE_VIEW_FLAG_ALLOW_MIP_MAP_GENERATION = 1u << 0,
    TEXTURE_VIEW_FLAG_LAST = TEXTURE_VIEW_FLAG_ALLOW_MIP_MAP_GENERATION
};
DEFINE_FLAG_ENUM_OPERATORS(TEXTURE_VIEW_FLAGS);

enum TEXTURE_COMPONENT_SWIZZLE : uint8_t
{
    TEXTURE_COMPONENT_SWIZZLE_IDENTITY = 0x00,

    TEXTURE_COMPONENT_SWIZZLE_ZERO,
    TEXTURE_COMPONENT_SWIZZLE_ONE,

    TEXTURE_COMPONENT_SWIZZLE_R,
    TEXTURE_COMPONENT_SWIZZLE_G,
    TEXTURE_COMPONENT_SWIZZLE_B,
    TEXTURE_COMPONENT_SWIZZLE_A,

    TEXTURE_COMPONENT_SWIZZLE_COUNT
};
DEFINE_FLAG_ENUM_OPERATORS(TEXTURE_COMPONENT_SWIZZLE);

struct TextureComponentMapping
{
    TEXTURE_COMPONENT_SWIZZLE R = TEXTURE_COMPONENT_SWIZZLE_IDENTITY;
    TEXTURE_COMPONENT_SWIZZLE G = TEXTURE_COMPONENT_SWIZZLE_IDENTITY;
    TEXTURE_COMPONENT_SWIZZLE B = TEXTURE_COMPONENT_SWIZZLE_IDENTITY;
    TEXTURE_COMPONENT_SWIZZLE A = TEXTURE_COMPONENT_SWIZZLE_IDENTITY;

    constexpr TextureComponentMapping() noexcept {};

    constexpr TextureComponentMapping(TEXTURE_COMPONENT_SWIZZLE _r,
                                      TEXTURE_COMPONENT_SWIZZLE _g,
                                      TEXTURE_COMPONENT_SWIZZLE _b,
                                      TEXTURE_COMPONENT_SWIZZLE _a) noexcept
        : R(_r), G(_g), B(_b), A(_a) {}

    constexpr uint32_t AsUint32()
    {
        return (static_cast<uint32_t>(R) <<  0u) |
               (static_cast<uint32_t>(G) <<  8u) |
               (static_cast<uint32_t>(B) << 16u) |
               (static_cast<uint32_t>(A) << 24u);
    }

    constexpr bool operator==(const TextureComponentMapping& _rhs) const
    {
        return (R == _rhs.R || (R == TEXTURE_COMPONENT_SWIZZLE_IDENTITY && _rhs.R == TEXTURE_COMPONENT_SWIZZLE_R) || (R == TEXTURE_COMPONENT_SWIZZLE_R && _rhs.R == TEXTURE_COMPONENT_SWIZZLE_IDENTITY)) &&
               (G == _rhs.G || (G == TEXTURE_COMPONENT_SWIZZLE_IDENTITY && _rhs.G == TEXTURE_COMPONENT_SWIZZLE_G) || (G == TEXTURE_COMPONENT_SWIZZLE_G && _rhs.G == TEXTURE_COMPONENT_SWIZZLE_IDENTITY)) &&
               (B == _rhs.B || (B == TEXTURE_COMPONENT_SWIZZLE_IDENTITY && _rhs.B == TEXTURE_COMPONENT_SWIZZLE_B) || (B == TEXTURE_COMPONENT_SWIZZLE_B && _rhs.B == TEXTURE_COMPONENT_SWIZZLE_IDENTITY)) &&
               (A == _rhs.A || (A == TEXTURE_COMPONENT_SWIZZLE_IDENTITY && _rhs.A == TEXTURE_COMPONENT_SWIZZLE_A) || (A == TEXTURE_COMPONENT_SWIZZLE_A && _rhs.A == TEXTURE_COMPONENT_SWIZZLE_IDENTITY));
    }

    constexpr bool operator!=(const TextureComponentMapping& _rhs) const
    {
        return !(*this == _rhs);
    }

    constexpr TEXTURE_COMPONENT_SWIZZLE operator[](size_t _component) const
	{
		return (&R)[_component];
	}

    constexpr TEXTURE_COMPONENT_SWIZZLE& operator[](size_t _component)
    {
    		return (&R)[_component];
	}

    static constexpr TextureComponentMapping Identity()
	{
		return {
            TEXTURE_COMPONENT_SWIZZLE_IDENTITY,
            TEXTURE_COMPONENT_SWIZZLE_IDENTITY,
            TEXTURE_COMPONENT_SWIZZLE_IDENTITY,
            TEXTURE_COMPONENT_SWIZZLE_IDENTITY
        };
    }

    // Combines two component mappings into one.
    // The resulting mapping is equivalent to first applying the first (lhs) mapping,
    // then applying the second (rhs) mapping.
    TextureComponentMapping operator*(const TextureComponentMapping& _rhs) const
    {
        TextureComponentMapping combinedMapping;
        for (size_t c = 0; c < 4; ++c)
        {
            TEXTURE_COMPONENT_SWIZZLE  rhsCompSwizzle = _rhs[c];
            TEXTURE_COMPONENT_SWIZZLE& dstCompSwizzle = combinedMapping[c];
            switch (rhsCompSwizzle)
            {
                case TEXTURE_COMPONENT_SWIZZLE_IDENTITY: dstCompSwizzle = (*this)[c]; break;
                case TEXTURE_COMPONENT_SWIZZLE_ZERO:     dstCompSwizzle = TEXTURE_COMPONENT_SWIZZLE_ZERO; break;
                case TEXTURE_COMPONENT_SWIZZLE_ONE:      dstCompSwizzle = TEXTURE_COMPONENT_SWIZZLE_ONE; break;
                case TEXTURE_COMPONENT_SWIZZLE_R:        dstCompSwizzle = (R == TEXTURE_COMPONENT_SWIZZLE_IDENTITY) ? TEXTURE_COMPONENT_SWIZZLE_R : R; break;
                case TEXTURE_COMPONENT_SWIZZLE_G:        dstCompSwizzle = (G == TEXTURE_COMPONENT_SWIZZLE_IDENTITY) ? TEXTURE_COMPONENT_SWIZZLE_G : G; break;
                case TEXTURE_COMPONENT_SWIZZLE_B:        dstCompSwizzle = (B == TEXTURE_COMPONENT_SWIZZLE_IDENTITY) ? TEXTURE_COMPONENT_SWIZZLE_B : B; break;
                case TEXTURE_COMPONENT_SWIZZLE_A:        dstCompSwizzle = (A == TEXTURE_COMPONENT_SWIZZLE_IDENTITY) ? TEXTURE_COMPONENT_SWIZZLE_A : A; break;
                default: dstCompSwizzle = (*this)[c]; break;
            }

            if ((dstCompSwizzle == TEXTURE_COMPONENT_SWIZZLE_R && c == 0) ||
                (dstCompSwizzle == TEXTURE_COMPONENT_SWIZZLE_G && c == 1) ||
                (dstCompSwizzle == TEXTURE_COMPONENT_SWIZZLE_B && c == 2) ||
                (dstCompSwizzle == TEXTURE_COMPONENT_SWIZZLE_A && c == 3))
            {
                dstCompSwizzle = TEXTURE_COMPONENT_SWIZZLE_IDENTITY;
            }
        }
        static_assert(TEXTURE_COMPONENT_SWIZZLE_COUNT == 7, "Please handle the new component swizzle");
        return combinedMapping;
    }

    TextureComponentMapping& operator*=(const TextureComponentMapping& _rhs)
    {
        *this = *this * _rhs;
        return *this;
    }
};

struct TextureViewDesc : public DeviceObjectAttribs
{
    TEXTURE_VIEW_TYPE ViewType = TEXTURE_VIEW_UNDEFINED;

    RESOURCE_DIMENSION TextureDim = RESOURCE_DIM_UNDEFINED;

    TEXTURE_FORMAT Format = TEX_FORMAT_UNKNOWN;

    uint32_t MostDetailedMip = 0;

    uint32_t NumMipLevels = 0;

    union
    {
        uint32_t FirstArraySlice = 0;
        uint32_t FirstDepthSlice;
    };

    union
    {
        uint32_t NumArraySlices = 0;
        uint32_t NumDepthSlices;
    };

    UAV_ACCESS_FLAG AccessFlags = UAV_ACCESS_UNSPECIFIED;
    TEXTURE_VIEW_FLAGS Flags = TEXTURE_VIEW_FLAG_NONE;

    TextureComponentMapping Swizzle;

    constexpr TextureViewDesc() noexcept {}

    constexpr TextureViewDesc(const char*        _name,
                              TEXTURE_VIEW_TYPE  _viewType,
                              RESOURCE_DIMENSION _textureDim,
                              TEXTURE_FORMAT     _format                 = TextureViewDesc{}.Format,
                              uint32_t           _mostDetailedMip        = TextureViewDesc{}.MostDetailedMip,
                              uint32_t           _numMipLevels           = TextureViewDesc{}.NumMipLevels,
                              uint32_t           _firstArrayOrDepthSlice = TextureViewDesc{}.FirstArraySlice,
                              uint32_t           _numArrayOrDepthSlices  = TextureViewDesc{}.NumArraySlices,
                              UAV_ACCESS_FLAG    _accessFlags            = TextureViewDesc{}.AccessFlags,
                              TEXTURE_VIEW_FLAGS _flags                  = TextureViewDesc{}.Flags) noexcept :
        DeviceObjectAttribs{_name                  },
        ViewType           {_viewType              },
        TextureDim         {_textureDim            },
        Format             {_format                },
        MostDetailedMip    {_mostDetailedMip       },
        NumMipLevels       {_numMipLevels          },
        FirstArraySlice    {_firstArrayOrDepthSlice},
        NumArraySlices     {_numArrayOrDepthSlices },
        AccessFlags        {_accessFlags           },
        Flags              {_flags                 }
    {}

    constexpr uint32_t FirstArrayOrDepthSlice() const { return FirstArraySlice; }
    constexpr uint32_t NumArrayOrDepthSlices()  const { return NumArraySlices; }

    /// Tests if two texture view descriptions are equal.

    /// \param [in] _rhs - reference to the structure to compare with.
    ///
    /// \return     true if all members of the two structures *except for the Name* are equal,
    ///             and false otherwise.
    ///
    /// \note   The operator ignores the Name field as it is used for debug purposes and
    ///         doesn't affect the texture view properties.
    constexpr bool operator==(const TextureViewDesc& _rhs) const
    {
        // Ignore Name. This is consistent with the hasher (HashCombiner<HasherType, TextureViewDesc>).
        return //strcmp(Name, RHS.Name) == 0            &&
            ViewType                 == _rhs.ViewType                 &&
            TextureDim               == _rhs.TextureDim               &&
            Format                   == _rhs.Format                   &&
            MostDetailedMip          == _rhs.MostDetailedMip          &&
            NumMipLevels             == _rhs.NumMipLevels             &&
            FirstArrayOrDepthSlice() == _rhs.FirstArrayOrDepthSlice() &&
            NumArrayOrDepthSlices()  == _rhs.NumArrayOrDepthSlices()  &&
            AccessFlags              == _rhs.AccessFlags              &&
            Flags                    == _rhs.Flags                    &&
            Swizzle                  == _rhs.Swizzle;
    }
    
    constexpr bool operator!=(const TextureViewDesc& _rhs) const
    {
        return !(*this == _rhs);
    }
};

struct ITextureView : public IDeviceObject
{
    virtual const TextureViewDesc& GetDesc() const override = 0;

    virtual void             SetSampler(struct ISampler* _pSampler) = 0;
    virtual struct ISampler* GetSampler() = 0;

    virtual struct ITexture* GetTexture() const = 0;
};

}
