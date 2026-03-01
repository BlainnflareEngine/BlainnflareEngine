//
// Created by WhoLeb on 01-Mar-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once

#include "ISampler.h"
#include "IShader.h"
#include "IShaderResourceBinding.h"
#include "IShaderResourceVariable.h"
#include "Render/GraphicsTypes.h"

namespace Blainn
{
/// Immutable sampler description.

/// An immutable sampler is compiled into the pipeline state and can't be changed.
/// It is generally more efficient than a regular sampler and should be used
/// whenever possible.
struct ImmutableSamplerDesc
{
    /// Shader stages that this immutable sampler applies to. More than one shader stage can be specified.
    SHADER_TYPE ShaderStages         = SHADER_TYPE_UNKNOWN);

    /// The name of the sampler itself or the name of the texture variable that
    /// this immutable sampler is assigned to if combined texture samplers are used.
    const char* SamplerOrTextureName = nullptr;

    /// Sampler description
    struct SamplerDesc Desc;

    constexpr ImmutableSamplerDesc() noexcept {}

    constexpr ImmutableSamplerDesc(SHADER_TYPE        _shaderStages,
                                   const char*        _samplerOrTextureName,
                                   const SamplerDesc& _desc)noexcept :
        ShaderStages        {_shaderStages        },
        SamplerOrTextureName{_samplerOrTextureName},
        Desc                {_desc                }
    {}

    bool operator==(const ImmutableSamplerDesc& _rhs) const noexcept
    {
        return ShaderStages == _rhs.ShaderStages &&
               Desc         == _rhs.Desc &&
               SafeStrEqual(SamplerOrTextureName, _rhs.SamplerOrTextureName);
    }
    
    bool operator!=(const ImmutableSamplerDesc& _rhs) const noexcept
    {
        return !(*this == _rhs);
    }
};


enum PIPELINE_RESOURCE_FLAGS : uint8_t
{
    /// Resource has no special properties
    PIPELINE_RESOURCE_FLAG_NONE            = 0,

    /// Indicates that dynamic buffers will never be bound to the resource
    /// variable. Applies to SHADER_RESOURCE_TYPE_CONSTANT_BUFFER,
    /// SHADER_RESOURCE_TYPE_BUFFER_UAV, SHADER_RESOURCE_TYPE_BUFFER_SRV resources.
    ///
    /// In Vulkan and Direct3D12 backends, dynamic buffers require extra work
    /// at run time. If an application knows it will never bind a dynamic buffer to
    /// the variable, it should use PIPELINE_RESOURCE_FLAG_NO_DYNAMIC_BUFFERS flag
    /// to improve performance. This flag is not required and non-dynamic buffers
    /// will still work even if the flag is not used. It is an error to bind a
    /// dynamic buffer to resource that uses
    /// PIPELINE_RESOURCE_FLAG_NO_DYNAMIC_BUFFERS flag.
    PIPELINE_RESOURCE_FLAG_NO_DYNAMIC_BUFFERS = 1u << 0,

    /// Indicates that the resource consists of inline constants (also
    /// known as push constants in Vulkan or root constants in Direct3D12).
    ///
    /// Applies to SHADER_RESOURCE_TYPE_CONSTANT_BUFFER only.
    ///
    /// Use this flag if you have a buffer of frequently changing constants
    /// - that are small in size (typically up to 128 bytes) and
    /// - change often (e.g. per-draw or per-dispatch).
    ///
    /// Inline constants are set directly using IShaderResourceVariable::SetInlineConstants.
    ///
    /// This flag cannot be combined with any other flags.
    ///
    /// In Vulkan and Direct3D12, inline constants are not bound via descriptor sets or root
    /// signatures, but are set directly in command buffers or command lists and are very cheap.
    /// In legacy APIs (Direct3D11 and OpenGL), inline constants are emulated using regular
    /// constant buffers and thus have higher overhead.
    PIPELINE_RESOURCE_FLAG_INLINE_CONSTANTS = 1u << 1,

    /// Indicates that a texture SRV will be combined with a sampler.
    /// Applies to SHADER_RESOURCE_TYPE_TEXTURE_SRV resources.
    PIPELINE_RESOURCE_FLAG_COMBINED_SAMPLER   = 1u << 2,

    /// Indicates that this variable will be used to bind formatted buffers.
    /// Applies to SHADER_RESOURCE_TYPE_BUFFER_UAV and SHADER_RESOURCE_TYPE_BUFFER_SRV
    /// resources.
    ///
    /// In Vulkan backend formatted buffers require another descriptor type
    /// as opposed to structured buffers. If an application will be using
    /// formatted buffers with buffer UAVs and SRVs, it must specify the
    /// PIPELINE_RESOURCE_FLAG_FORMATTED_BUFFER flag.
    PIPELINE_RESOURCE_FLAG_FORMATTED_BUFFER   = 1u << 3,

    /// Indicates that resource is a run-time sized shader array (e.g. an array without a specific size).
    PIPELINE_RESOURCE_FLAG_RUNTIME_ARRAY      = 1u << 4,

    /// Indicates that the resource is an input attachment in general layout, which allows simultaneously
    /// reading from the resource through the input attachment and writing to it via color or depth-stencil
    /// attachment.
    ///
    /// \note This flag is only valid in Vulkan.
    PIPELINE_RESOURCE_FLAG_GENERAL_INPUT_ATTACHMENT = 1u << 5,

    PIPELINE_RESOURCE_FLAG_LAST = PIPELINE_RESOURCE_FLAG_GENERAL_INPUT_ATTACHMENT
};
DEFINE_FLAG_ENUM_OPERATORS(PIPELINE_RESOURCE_FLAGS);


/// Pipeline resource description.
struct PipelineResourceDesc
{
    /// Resource name in the shader
    const char*                    Name          = nullptr;

    /// Shader stages that this resource applies to.

    /// When multiple shader stages are specified, all stages will share the same resource.
    ///
    /// There may be multiple resources with the same name in different shader stages,
    /// but the stages specified for different resources with the same name must not overlap.
    SHADER_TYPE                    ShaderStages  = SHADER_TYPE_UNKNOWN;

    /// Resource array size (must be 1 for non-array resources).
    ///
    /// For inline constants (see PIPELINE_RESOURCE_FLAG_INLINE_CONSTANTS),
    /// this member specifies the number of 4-byte values.
    uint32_t                      ArraySize     = 1;

    /// Resource type, see SHADER_RESOURCE_TYPE.
    SHADER_RESOURCE_TYPE          ResourceType  = SHADER_RESOURCE_TYPE_UNKNOWN;

    /// Resource variable type, see SHADER_RESOURCE_VARIABLE_TYPE.
    SHADER_RESOURCE_VARIABLE_TYPE VarType       = SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE;

    /// Special resource flags, see PIPELINE_RESOURCE_FLAGS.
    PIPELINE_RESOURCE_FLAGS       Flags         = PIPELINE_RESOURCE_FLAG_NONE;

    constexpr PipelineResourceDesc() noexcept {}

    constexpr PipelineResourceDesc(SHADER_TYPE                   _shaderStages,
                                   const char*                   _name,
                                   uint32_t                      _arraySize,
                                   SHADER_RESOURCE_TYPE          _resourceType,
                                   SHADER_RESOURCE_VARIABLE_TYPE _varType       = PipelineResourceDesc{}.VarType,
                                   PIPELINE_RESOURCE_FLAGS       _flags         = PipelineResourceDesc{}.Flags) noexcept 
        : Name         {_name         }
        , ShaderStages {_shaderStages }
        , ArraySize    {_arraySize    }
        , ResourceType {_resourceType }
        , VarType      {_varType      }
        , Flags        {_flags        }
    {}

    constexpr PipelineResourceDesc(SHADER_TYPE                   _shaderStages,
                                   const char*                   _name,
                                   SHADER_RESOURCE_TYPE          _resourceType,
                                   SHADER_RESOURCE_VARIABLE_TYPE _varType       = PipelineResourceDesc{}.VarType,
                                   PIPELINE_RESOURCE_FLAGS       _flags         = PipelineResourceDesc{}.Flags) noexcept
        : Name         {_name         }
        , ShaderStages {_shaderStages }
        , ResourceType {_resourceType }
        , VarType      {_varType      }
        , Flags        {_flags        }
    {}

    bool operator==(const PipelineResourceDesc& _rhs) const noexcept
    {
        return ShaderStages  == _rhs.ShaderStages  &&
               ArraySize     == _rhs.ArraySize     &&
               ResourceType  == _rhs.ResourceType  &&
               VarType       == _rhs.VarType       &&
               Flags         == _rhs.Flags         &&
               SafeStrEqual(Name, _rhs.Name);
    }
    bool operator!=(const PipelineResourceDesc& _rhs) const noexcept
    {
        return !(*this == _rhs);
    }

    uint32_t GetArraySize() const noexcept
    {
        return (Flags & PIPELINE_RESOURCE_FLAG_INLINE_CONSTANTS) != 0 ?
                1 : // For inline constants, ArraySize is the number of 4-byte constants.
                ArraySize;
    }
};

struct PipelineResourceSignatureDesc : DeviceObjectAttribs
{

    /// A pointer to an array of resource descriptions. See PipelineResourceDesc.
    const PipelineResourceDesc*  Resources              = nullptr;

    /// The number of resources in Resources array.
    uint32_t                     NumResources           = 0;

    /// A pointer to an array of immutable samplers. See ImmutableSamplerDesc.
    const ImmutableSamplerDesc*  ImmutableSamplers      = nullptr;

    /// The number of immutable samplers in ImmutableSamplers array.
    uint32_t                     NumImmutableSamplers  = 0;

    /// Binding index that this resource signature uses.

    /// Every resource signature must be assign to one signature slot.
    /// The total number of slots is given by MAX_RESOURCE_SIGNATURES constant.
    /// All resource signatures used by a pipeline state must be assigned
    /// to different slots.
    uint8_t                      BindingIndex = 0;

    /// Whether to use combined texture samplers.

    /// If set to true, textures will be combined with texture samplers.
    /// The `CombinedSamplerSuffix` member defines the suffix added to the texture variable
    /// name to get corresponding sampler name. When using combined samplers,
    /// the sampler assigned to the shader resource view is automatically set when
    /// the view is bound. Otherwise samplers need to be explicitly set similar to other
    /// shader variables.
    bool UseCombinedTextureSamplers = false;

    /// Combined sampler suffix.

    /// If `UseCombinedTextureSamplers` is `true`, defines the suffix added to the
    /// texture variable name to get corresponding sampler name.  For example,
    /// for default value "_sampler", a texture named "tex" will be combined
    /// with sampler named "tex_sampler".
    /// If `UseCombinedTextureSamplers` is `false`, this member is ignored.
    const char* CombinedSamplerSuffix = "_sampler";

    /// Shader resource binding allocation granularity

    /// This member defines the allocation granularity for internal resources required by
    /// the shader resource binding object instances.
    uint32_t SRBAllocationGranularity = 1;

    bool operator==(const PipelineResourceSignatureDesc& _rhs) const noexcept
    {
        // Ignore Name. This is consistent with the hasher (HashCombiner<HasherType, PipelineResourceSignatureDesc>).
        if (NumResources               != _rhs.NumResources         ||
            NumImmutableSamplers       != _rhs.NumImmutableSamplers ||
            BindingIndex               != _rhs.BindingIndex         ||
            UseCombinedTextureSamplers != _rhs.UseCombinedTextureSamplers ||
            SRBAllocationGranularity   != _rhs.SRBAllocationGranularity)
            return false;

        if (UseCombinedTextureSamplers && !SafeStrEqual(CombinedSamplerSuffix, _rhs.CombinedSamplerSuffix))
            return false;

        for (uint32_t r = 0; r < NumResources; ++r)
        {
            if (!(Resources[r] == _rhs.Resources[r]))
                return false;
        }
        for (uint32_t s = 0; s < NumImmutableSamplers; ++s)
        {
            if (!(ImmutableSamplers[s] == _rhs.ImmutableSamplers[s]))
                return false;
        }
        return true;
    }
    bool operator!=(const PipelineResourceSignatureDesc& _rhs) const
    {
        return !(*this == _rhs);
    }
};

struct IPipelineResourceSignature : public IDeviceObject
{
    virtual const PipelineResourceSignatureDesc& GetDesc() const override = 0;
    
    /// Creates a shader resource binding object

    /// \param [out] _ppShaderResourceBinding - Memory location where pointer to the new shader resource
    ///                                        binding object is written.
    /// \param [in] _initStaticResources      - If set to true, the method will initialize static resources in
    ///                                        the created object, which has the exact same effect as calling
    ///                                        IPipelineResourceSignature::InitializeStaticSRBResources().
    virtual void CreateShaderResourceBinding(IShaderResourceBinding** _ppShaderResourceBinding,
                                            bool                      _initStaticResources = false) = 0;


    /// Binds static resources for the specified shader stages in the pipeline resource signature.

    /// \param [in] _shaderStages     - Flags that specify shader stages, for which resources will be bound.
    ///                                Any combination of SHADER_TYPE may be used.
    /// \param [in] _pResourceMapping - Pointer to the resource mapping interface.
    /// \param [in] _flags            - Additional flags. See BIND_SHADER_RESOURCES_FLAGS.
    virtual void BindStaticResources(SHADER_TYPE                 _shaderStages,
                                     IResourceMapping*           _pResourceMapping,
                                     BIND_SHADER_RESOURCES_FLAGS _flags) = 0;


    /// Returns static shader resource variable. If the variable is not found,
    /// returns nullptr.

    /// \param [in] ShaderType - Type of the shader to look up the variable.
    ///                          Must be one of SHADER_TYPE.
    /// \param [in] Name       - Name of the variable.
    ///
    /// If a variable is shared between multiple shader stages,
    /// it can be accessed using any of those shader stages. Even
    /// though IShaderResourceVariable instances returned by the method
    /// may be different for different stages, internally they will
    /// reference the same resource.
    ///
    /// Only static shader resource variables can be accessed using this method.
    /// Mutable and dynamic variables are accessed through Shader Resource
    /// Binding object.
    ///
    /// The method does not increment the reference counter of the
    /// returned interface, and the application must *not* call Release()
    /// unless it explicitly called AddRef().
    virtual IShaderResourceVariable* GetStaticVariableByName(SHADER_TYPE ShaderType,
                                                             const char* Name) = 0;


    /// Returns static shader resource variable by its index.

    /// \param [in] ShaderType - Type of the shader to look up the variable.
    ///                          Must be one of SHADER_TYPE.
    /// \param [in] Index      - Shader variable index. The index must be between
    ///                          0 and the total number of variables returned by
    ///                          GetStaticVariableCount().
    ///
    ///
    /// If a variable is shared between multiple shader stages,
    /// it can be accessed using any of those shader stages. Even
    /// though IShaderResourceVariable instances returned by the method
    /// may be different for different stages, internally they will
    /// reference the same resource.
    ///
    /// Only static shader resource variables can be accessed using this method.
    /// Mutable and dynamic variables are accessed through Shader Resource
    /// Binding object.
    ///
    /// The method does not increment the reference counter of the
    /// returned interface, and the application must *not* call Release()
    /// unless it explicitly called AddRef().
    virtual IShaderResourceVariable* GetStaticVariableByIndex(SHADER_TYPE ShaderType,
                                                              uint32_t      Index) = 0;


    /// Returns the number of static shader resource variables.

    /// \param [in] _shaderType - Type of the shader.
    ///
    /// \remarks   Only static variables (that can be accessed directly through the PSO) are counted.
    ///            Mutable and dynamic variables are accessed through Shader Resource Binding object.
    virtual uint32_t GetStaticVariableCount(SHADER_TYPE _shaderType) const = 0;

    /// Initializes static resources in the shader binding object.

    /// If static shader resources were not initialized when the SRB was created,
    /// this method must be called to initialize them before the SRB can be used.
    /// The method should be called after all static variables have been initialized
    /// in the signature.
    ///
    /// \param [in] _pShaderResourceBinding - Shader resource binding object to initialize.
    ///                                      The pipeline resource signature must be compatible
    ///                                      with the shader resource binding object.
    ///
    /// If static resources have already been initialized in the SRB and the method
    /// is called again, it will have no effect and a warning message will be displayed.
    virtual void InitializeStaticSRBResources(struct IShaderResourceBinding* _pShaderResourceBinding) const = 0;

    /// Copies static resource bindings to the destination signature.

    /// \param [in] _pDstSignature - Destination pipeline resource signature.
    ///
    /// \note   Destination signature must be compatible with this signature.
    virtual void CopyStaticResources(IPipelineResourceSignature* _pDstSignature) const = 0;

    /// Returns true if the signature is compatible with another one.

    /// Two signatures are compatible if they contain identical resources and immutable samplers,
    /// defined in the same order disregarding their names.
    virtual bool IsCompatibleWith(const struct IPipelineResourceSignature* _pPRS) const = 0;
};

} // namespace Blainn