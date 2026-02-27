//
// Created by WhoLeb on 24-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once

#include "Render/GraphicsTypes.h"
#include "BlendState.h"
#include "DepthStencilState.h"
#include "InputLayout.h"
#include "RasterizerState.h"
#include "IShader.h"

namespace Blainn
{

struct SampleDesc
{
    uint8_t Count   = 1;
    uint8_t Quality = 0;

    constexpr SampleDesc() noexcept {}

    constexpr SampleDesc(uint8_t _count, uint8_t _quality) noexcept :
        Count   {_count  },
        Quality {_quality}
    {}

    constexpr bool operator==(const SampleDesc& _rhs) const
    {
        return Count == _rhs.Count && Quality == _rhs.Quality;
    }

    constexpr bool operator!=(const SampleDesc& _rhs) const
    {
        return !(*this == _rhs);
    }
};

enum SHADER_VARIABLE_FLAGS : uint8_t
{
    /// Shader variable has no special properties.
    SHADER_VARIABLE_FLAG_NONE               = 0,

    /// Indicates that dynamic buffers will never be bound to the resource
    /// variable. Applies to Diligent::SHADER_RESOURCE_TYPE_CONSTANT_BUFFER,
    /// Diligent::SHADER_RESOURCE_TYPE_BUFFER_UAV, Diligent::SHADER_RESOURCE_TYPE_BUFFER_SRV resources.
    ///
    /// \remarks    This flag directly translates to the Diligent::PIPELINE_RESOURCE_FLAG_NO_DYNAMIC_BUFFERS
    ///             flag in the internal pipeline resource signature.
    SHADER_VARIABLE_FLAG_NO_DYNAMIC_BUFFERS = 1u << 0,

    /// Indicates that the resource consists of inline constants
    /// (also known as push constants in Vulkan or root constants in Direct3D12).
    /// Applies to Diligent::SHADER_RESOURCE_TYPE_CONSTANT_BUFFER only.
    ///
    /// \remarks    This flag directly translates to the Diligent::PIPELINE_RESOURCE_FLAG_INLINE_CONSTANTS
    ///             flag in the internal pipeline resource signature.
    ///
    /// See Diligent::PIPELINE_RESOURCE_FLAG_INLINE_CONSTANTS for more details.
    SHADER_VARIABLE_FLAG_INLINE_CONSTANTS = 1u << 1,

    /// Indicates that the resource is an input attachment in general layout, which allows simultaneously
    /// reading from the resource through the input attachment and writing to it via color or depth-stencil
    /// attachment.
    ///
    /// \note This flag is only valid in Vulkan.
    SHADER_VARIABLE_FLAG_GENERAL_INPUT_ATTACHMENT_VK = 1u << 2,

    /// Indicates that the resource is an unfilterable-float texture.
    ///
    /// \note This flag is only valid in WebGPU and ignored in other backends.
    SHADER_VARIABLE_FLAG_UNFILTERABLE_FLOAT_TEXTURE_WEBGPU = 1u << 3,

    /// Indicates that the resource is a non-filtering sampler.
    ///
    /// \note This flag is only valid in WebGPU and ignored in other backends.
    SHADER_VARIABLE_FLAG_NON_FILTERING_SAMPLER_WEBGPU = 1u << 4,

    /// Special value that indicates the last flag in the enumeration.
    SHADER_VARIABLE_FLAG_LAST = SHADER_VARIABLE_FLAG_NON_FILTERING_SAMPLER_WEBGPU
};
DEFINE_FLAG_ENUM_OPERATORS(SHADER_VARIABLE_FLAGS);


/// Describes shader variable
struct ShaderResourceVariableDesc
{
    /// Shader variable name
    const Char*                   Name         DEFAULT_INITIALIZER(nullptr);

    /// Shader stages this resources variable applies to.

    /// If more than one shader stage is specified, the variable will be shared between these stages.
    /// Shader stages used by different variables with the same name must not overlap.
    SHADER_TYPE                   ShaderStages DEFAULT_INITIALIZER(SHADER_TYPE_UNKNOWN);

    /// Shader variable type. See Diligent::SHADER_RESOURCE_VARIABLE_TYPE for a list of allowed types
    SHADER_RESOURCE_VARIABLE_TYPE Type         DEFAULT_INITIALIZER(SHADER_RESOURCE_VARIABLE_TYPE_STATIC);

    SHADER_VARIABLE_FLAGS         Flags        DEFAULT_INITIALIZER(SHADER_VARIABLE_FLAG_NONE);

#if DILIGENT_CPP_INTERFACE
    constexpr ShaderResourceVariableDesc() noexcept {}

    constexpr ShaderResourceVariableDesc(SHADER_TYPE                   _ShaderStages,
                                         const Char*                   _Name,
                                         SHADER_RESOURCE_VARIABLE_TYPE _Type,
                                         SHADER_VARIABLE_FLAGS         _Flags = SHADER_VARIABLE_FLAG_NONE) noexcept :
        Name        {_Name        },
        ShaderStages{_ShaderStages},
        Type        {_Type        },
        Flags       {_Flags       }
    {}

    /// Comparison operator tests if two structures are equivalent

    /// \param [in] RHS - reference to the structure to perform comparison with
    /// \return
    /// - True if all members of the two structures are equal.
    /// - False otherwise.
    bool operator==(const ShaderResourceVariableDesc& RHS) const noexcept
    {
        return ShaderStages == RHS.ShaderStages &&
               Type         == RHS.Type         &&
               Flags        == RHS.Flags        &&
               SafeStrEqual(Name, RHS.Name);
    }

    bool operator!=(const ShaderResourceVariableDesc& RHS) const noexcept
    {
        return !(*this == RHS);
    }
#endif
};

enum PIPELINE_SHADING_RATE_FLAGS : uint8_t
{
    /// Shading rate is not used.
    PIPELINE_SHADING_RATE_FLAG_NONE          = 0,

    /// Indicates that the pipeline state will be used with per draw or per primitive shading rate.
    /// See IDeviceContext::SetShadingRate().
    PIPELINE_SHADING_RATE_FLAG_PER_PRIMITIVE = 1u << 0u,

    /// Indicates that the pipeline state will be used with texture-based shading rate.
    /// See IDeviceContext::SetShadingRate() and IDeviceContext::SetRenderTargetsExt().
    PIPELINE_SHADING_RATE_FLAG_TEXTURE_BASED = 1u << 1u,

    /// Special value that indicates the last flag in the enumeration.
    PIPELINE_SHADING_RATE_FLAG_LAST          = PIPELINE_SHADING_RATE_FLAG_TEXTURE_BASED,
};
DEFINE_FLAG_ENUM_OPERATORS(PIPELINE_SHADING_RATE_FLAGS);

/// Pipeline layout description
struct PipelineResourceLayoutDesc
{
    /// Default shader resource variable type.

    /// This type will be used if shader variable description is not found in the `Variables` array
    /// or if `Variables == nullptr`
    SHADER_RESOURCE_VARIABLE_TYPE       DefaultVariableType  DEFAULT_INITIALIZER(SHADER_RESOURCE_VARIABLE_TYPE_STATIC);

    /// Default shader variable merge stages.

    /// By default, all variables not found in the Variables array define separate resources.
    /// For example, if there is resource `"g_Texture"` in the vertex and pixel shader stages, there
    /// will be two separate resources in both stages. This member defines shader stages
    /// in which default variables will be combined.
    /// For example, if `DefaultVariableMergeStages == SHADER_TYPE_VERTEX | SHADER_TYPE_PIXEL`,
    /// then both resources in the example above will be combined into a single one.
    /// If there is another `"g_Texture"` in geometry shader, it will be separate from combined
    /// vertex-pixel "g_Texture".
    /// This member has no effect on variables defined in Variables array.
    SHADER_TYPE                         DefaultVariableMergeStages DEFAULT_INITIALIZER(SHADER_TYPE_UNKNOWN);

    /// Number of elements in Variables array
    Uint32                              NumVariables         DEFAULT_INITIALIZER(0);

    /// Array of shader resource variable descriptions

    /// There may be multiple variables with the same name that use different shader stages,
    /// but the stages must not overlap.
    const ShaderResourceVariableDesc*   Variables            DEFAULT_INITIALIZER(nullptr);

    /// Number of immutable samplers in ImmutableSamplers array
    Uint32                              NumImmutableSamplers DEFAULT_INITIALIZER(0);

    /// Array of immutable sampler descriptions
    const ImmutableSamplerDesc*         ImmutableSamplers    DEFAULT_INITIALIZER(nullptr);

#if DILIGENT_CPP_INTERFACE
    /// Returns true if two resource layout descriptions are equal, and false otherwise.
    /// Optionally ignores variable descriptions and/or sampler descriptions.
    static bool IsEqual(const PipelineResourceLayoutDesc& Desc1,
                        const PipelineResourceLayoutDesc& Desc2,
                        bool                              IgnoreVariables = false,
                        bool                              IgnoreSamplers  = false)
    {
        if (!(Desc1.DefaultVariableType        == Desc2.DefaultVariableType        &&
              Desc1.DefaultVariableMergeStages == Desc2.DefaultVariableMergeStages &&
              Desc1.NumVariables               == Desc2.NumVariables               &&
              Desc1.NumImmutableSamplers       == Desc2.NumImmutableSamplers))
           return false;

        if (!IgnoreVariables)
        {
            for (Uint32 i = 0; i < Desc1.NumVariables; ++i)
                if (Desc1.Variables[i] != Desc2.Variables[i])
                    return false;
        }

        if (!IgnoreSamplers)
        {
            for (Uint32 i = 0; i < Desc1.NumImmutableSamplers; ++i)
                if (Desc1.ImmutableSamplers[i] != Desc2.ImmutableSamplers[i])
                    return false;
        }

        return true;
    }

    /// Comparison operator tests if two structures are equivalent
    bool operator==(const PipelineResourceLayoutDesc& RHS) const noexcept
    {
        return IsEqual(*this, RHS);
    }
    bool operator!=(const PipelineResourceLayoutDesc& RHS) const noexcept
    {
        return !(*this == RHS);
    }
#endif

};

} // namespace Blainn