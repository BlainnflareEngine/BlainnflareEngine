//
// Created by WhoLeb on 01-Mar-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once

#include "IDeviceObject.h"

namespace Blainn
{
enum SAMPLER_FLAGS : uint8_t
{
    /// No flags are set.
    SAMPLER_FLAG_NONE        = 0,

    /// Specifies that the sampler will read from a subsampled texture created with MISC_TEXTURE_FLAG_SUBSAMPLED flag.
    /// Requires SHADING_RATE_CAP_FLAG_SUBSAMPLED_RENDER_TARGET capability.
    SAMPLER_FLAG_SUBSAMPLED  = 1u << 0,

    /// Specifies that the GPU is allowed to use fast approximation when reconstructing full-resolution value from
    /// the subsampled texture accessed by the sampler.
    /// Requires SHADING_RATE_CAP_FLAG_SUBSAMPLED_RENDER_TARGET capability.
    SAMPLER_FLAG_SUBSAMPLED_COARSE_RECONSTRUCTION = 1u << 1,

    SAMPLER_FLAG_LAST = SAMPLER_FLAG_SUBSAMPLED_COARSE_RECONSTRUCTION
};
DEFINE_FLAG_ENUM_OPERATORS(SAMPLER_FLAGS)

/// Sampler description

/// This structure describes the sampler state which is used in a call to
/// IRenderDevice::CreateSampler() to create a sampler object.
///
/// To create an anisotropic filter, all three filters must either be FILTER_TYPE_ANISOTROPIC
/// or FILTER_TYPE_COMPARISON_ANISOTROPIC.
///
/// `MipFilter` cannot be comparison filter except for FILTER_TYPE_ANISOTROPIC if all
/// three filters have that value.
///
/// Both `MinFilter` and `MagFilter` must either be regular filters or comparison filters.
/// Mixing comparison and regular filters is an error.
struct SamplerDesc : public DeviceObjectAttribs
{
    /// Texture minification filter, see FILTER_TYPE for details.

    /// Default value: FILTER_TYPE_LINEAR.
    FILTER_TYPE MinFilter           = FILTER_TYPE_LINEAR;

    /// Texture magnification filter, see FILTER_TYPE for details.

    /// Default value: FILTER_TYPE_LINEAR.
    FILTER_TYPE MagFilter           = FILTER_TYPE_LINEAR;

    /// Mip filter, see FILTER_TYPE for details.

    /// Only FILTER_TYPE_POINT, FILTER_TYPE_LINEAR, FILTER_TYPE_ANISOTROPIC, and
    /// FILTER_TYPE_COMPARISON_ANISOTROPIC are allowed.
    /// Default value: FILTER_TYPE_LINEAR.
    FILTER_TYPE MipFilter           = FILTER_TYPE_LINEAR;

    /// Texture address mode for U coordinate, see TEXTURE_ADDRESS_MODE for details
    /// Default value: TEXTURE_ADDRESS_CLAMP.
    TEXTURE_ADDRESS_MODE AddressU   = TEXTURE_ADDRESS_CLAMP;

    /// Texture address mode for V coordinate, see TEXTURE_ADDRESS_MODE for details

    /// Default value: TEXTURE_ADDRESS_CLAMP.
    TEXTURE_ADDRESS_MODE AddressV   = TEXTURE_ADDRESS_CLAMP;

    /// Texture address mode for W coordinate, see TEXTURE_ADDRESS_MODE for details

    /// Default value: TEXTURE_ADDRESS_CLAMP.
    TEXTURE_ADDRESS_MODE AddressW   = TEXTURE_ADDRESS_CLAMP;

    /// Sampler flags, see SAMPLER_FLAGS for details.
    SAMPLER_FLAGS        Flags      = SAMPLER_FLAG_NONE;

    /// Indicates whether to use unnormalized texture coordinates.

    /// When set to `True`, the range of the image coordinates used to lookup
    /// the texel is in the range of 0 to the image size in each dimension.
    /// When set to `False`, the range of image coordinates is 0.0 to 1.0.
    ///
    /// Unnormalized coordinates are only supported in Vulkan and Metal.
    bool    UnnormalizedCoords          = false;

    /// Offset from the calculated mipmap level.

    /// For example, if a sampler calculates that a texture should be sampled at mipmap
    /// level 1.2 and MipLODBias is 2.3, then the texture will be sampled at
    /// mipmap level 3.5.
    ///
    /// Default value: 0.
    float MipLODBias                      = 0;

    /// Maximum anisotropy level for the anisotropic filter. Default value: 0.
    uint32_t MaxAnisotropy                = 0;

    /// A function that compares sampled data against existing sampled data when comparison filter is used.

    /// Default value: COMPARISON_FUNC_NEVER.
    COMPARISON_FUNCTION ComparisonFunc  = COMPARISON_FUNC_NEVER);

    /// Border color to use if TEXTURE_ADDRESS_BORDER is specified for `AddressU`, `AddressV`, or `AddressW`.

    /// Default value: `{0, 0, 0, 0}`
    float BorderColor[4]                = {};

    /// Specifies the minimum value that LOD is clamped to before accessing the texture MIP levels.

    /// Must be less than or equal to `MaxLOD`.
    ///
    /// Default value: 0.
    float MinLOD                        = 0;

    /// Specifies the maximum value that LOD is clamped to before accessing the texture MIP levels.

    /// Must be greater than or equal to `MinLOD`.
    /// Default value: `+FLT_MAX`.
    float MaxLOD                        = +3.402823466e+38F;

    // 
    // NB: when adding new members, don't forget to update std::hash<SamplerDesc>
    //

    constexpr SamplerDesc() noexcept {}

    constexpr SamplerDesc(FILTER_TYPE          _minFilter,
                          FILTER_TYPE          _magFilter,
                          FILTER_TYPE          _mipFilter,
                          TEXTURE_ADDRESS_MODE _addressU           = SamplerDesc{}.AddressU,
                          TEXTURE_ADDRESS_MODE _addressV           = SamplerDesc{}.AddressV,
                          TEXTURE_ADDRESS_MODE _addressW           = SamplerDesc{}.AddressW,
                          float                _mipLODBias         = SamplerDesc{}.MipLODBias,
                          uint32_t             _maxAnisotropy      = SamplerDesc{}.MaxAnisotropy,
                          COMPARISON_FUNCTION  _comparisonFunc     = SamplerDesc{}.ComparisonFunc,
                          float                _minLOD             = SamplerDesc{}.MinLOD,
                          float                _maxLOD             = SamplerDesc{}.MaxLOD,
                          SAMPLER_FLAGS        _flags              = SamplerDesc{}.Flags,
                          bool                 _unnormalizedCoords = SamplerDesc{}.UnnormalizedCoords)
        : MinFilter         {_minFilter         }
        , MagFilter         {_magFilter         }
        , MipFilter         {_mipFilter         }
        , AddressU          {_addressU          }
        , AddressV          {_addressV          }
        , AddressW          {_addressW          }
        , Flags             {_flags             }
        , UnnormalizedCoords{_unnormalizedCoords}
        , MipLODBias        {_mipLODBias        }
        , MaxAnisotropy     {_maxAnisotropy     }
        , ComparisonFunc    {_comparisonFunc    }
        , MinLOD            {_minLOD            }
        , MaxLOD            {_maxLOD            }
    {
        BorderColor[0] = BorderColor[1] = BorderColor[2] = BorderColor[3] = 0;
    }

    /// Tests if two sampler descriptions are equal.

    /// \param [in] _rhs - reference to the structure to compare with.
    ///
    /// \return     true if all members of the two structures *except for the Name* are equal,
    ///             and false otherwise.
    ///
    /// \note   The operator ignores the Name field as it is used for debug purposes and
    ///         doesn't affect the sampler properties.
    constexpr bool operator == (const SamplerDesc& _rhs)const
    {
                // Ignore Name. This is consistent with the hasher (HashCombiner<HasherType, SamplerDesc>).
        return  // strcmp(Name, RHS.Name) == 0          &&
                MinFilter          == _rhs.MinFilter          &&
                MagFilter          == _rhs.MagFilter          &&
                MipFilter          == _rhs.MipFilter          &&
                AddressU           == _rhs.AddressU           &&
                AddressV           == _rhs.AddressV           &&
                AddressW           == _rhs.AddressW           &&
                Flags              == _rhs.Flags              &&
                UnnormalizedCoords == _rhs.UnnormalizedCoords &&
                MipLODBias         == _rhs.MipLODBias         &&
                MaxAnisotropy      == _rhs.MaxAnisotropy      &&
                ComparisonFunc     == _rhs.ComparisonFunc     &&
                BorderColor[0]     == _rhs.BorderColor[0]     &&
                BorderColor[1]     == _rhs.BorderColor[1]     &&
                BorderColor[2]     == _rhs.BorderColor[2]     &&
                BorderColor[3]     == _rhs.BorderColor[3]     &&
                MinLOD             == _rhs.MinLOD             &&
                MaxLOD             == _rhs.MaxLOD;
    }
    constexpr bool operator != (const SamplerDesc& _rhs)const
    {
        return !(*this == _rhs);
    }
};

struct ISampler : public IDeviceObject
{
    virtual const SamplerDesc& GetDesc() const override = 0;
};
}