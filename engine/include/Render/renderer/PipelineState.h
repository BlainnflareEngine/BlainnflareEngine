//
// Created by WhoLeb on 24-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once

#include "BlendState.h"
#include "DepthStencilState.h"
#include "InputLayout.h"
#include "IPipelineResourceSignature.h"
#include "IPipelineStateCache.h"
#include "ISampler.h"
#include "IShader.h"
#include "IShaderResourceBinding.h"
#include "IShaderResourceVariable.h"
#include "RasterizerState.h"
#include "Render/GraphicsTypes.h"

namespace Blainn
{

enum SHADER_VARIABLE_FLAGS : uint8_t
{
    /// Shader variable has no special properties.
    SHADER_VARIABLE_FLAG_NONE               = 0,

    /// Indicates that dynamic buffers will never be bound to the resource
    /// variable. Applies to SHADER_RESOURCE_TYPE_CONSTANT_BUFFER,
    /// SHADER_RESOURCE_TYPE_BUFFER_UAV, SHADER_RESOURCE_TYPE_BUFFER_SRV resources.
    ///
    /// \remarks    This flag directly translates to the PIPELINE_RESOURCE_FLAG_NO_DYNAMIC_BUFFERS
    ///             flag in the internal pipeline resource signature.
    SHADER_VARIABLE_FLAG_NO_DYNAMIC_BUFFERS = 1u << 0,

    /// Indicates that the resource consists of inline constants
    /// (also known as push constants in Vulkan or root constants in Direct3D12).
    /// Applies to SHADER_RESOURCE_TYPE_CONSTANT_BUFFER only.
    ///
    /// \remarks    This flag directly translates to the PIPELINE_RESOURCE_FLAG_INLINE_CONSTANTS
    ///             flag in the internal pipeline resource signature.
    ///
    /// See PIPELINE_RESOURCE_FLAG_INLINE_CONSTANTS for more details.
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

enum PIPELINE_TYPE : uint8_t
{
    /// Graphics pipeline, which is used by IDeviceContext::Draw(), IDeviceContext::DrawIndexed(),
    /// IDeviceContext::DrawIndirect(), IDeviceContext::DrawIndexedIndirect().
    PIPELINE_TYPE_GRAPHICS,

    /// Compute pipeline, which is used by IDeviceContext::DispatchCompute(), IDeviceContext::DispatchComputeIndirect().
    PIPELINE_TYPE_COMPUTE,

    /// Mesh pipeline, which is used by IDeviceContext::DrawMesh(), IDeviceContext::DrawMeshIndirect().
    PIPELINE_TYPE_MESH,

    /// Ray tracing pipeline, which is used by IDeviceContext::TraceRays().
    PIPELINE_TYPE_RAY_TRACING,

    /// Tile pipeline, which is used by IDeviceContext::DispatchTile().
    PIPELINE_TYPE_TILE,

    /// Special value that indicates the last pipeline type in the enumeration.
    PIPELINE_TYPE_LAST = PIPELINE_TYPE_TILE,

    /// Number of pipeline types in the enumeration.
    PIPELINE_TYPE_COUNT,

    PIPELINE_TYPE_INVALID = 0xFF
};


enum PSO_CREATE_FLAGS : uint32_t
{
    PSO_CREATE_FLAG_NONE = 0u,

    /// Ignore missing variables.

    /// By default, the engine outputs a warning for every variable
    /// provided as part of the pipeline resource layout description
    /// that is not found in any of the designated shader stages.
    /// Use this flag to silence these warnings.
    PSO_CREATE_FLAG_IGNORE_MISSING_VARIABLES          = 1u << 0u,

    /// Ignore missing immutable samplers.

    /// By default, the engine outputs a warning for every immutable sampler
    /// provided as part of the pipeline resource layout description
    /// that is not found in any of the designated shader stages.
    /// Use this flag to silence these warnings.
    PSO_CREATE_FLAG_IGNORE_MISSING_IMMUTABLE_SAMPLERS = 1u << 1u,

    /// Do not remap shader resources when creating the pipeline.

    /// Resource bindings in all shaders must match the bindings expected
    /// by the PSO's resource signatures.
    PSO_CREATE_FLAG_DONT_REMAP_SHADER_RESOURCES       = 1u << 2u,

    /// Create the pipeline state asynchronously.

    /// When this flag is set to true and if the devices supports
    /// AsyncShaderCompilation feature, the pipeline will be created
    /// asynchronously in the background. An application should use
    /// the IPipelineState::GetStatus() method to check the pipeline status.
    /// If the device does not support asynchronous shader compilation,
    /// the flag is ignored and the pipeline is created synchronously.
    PSO_CREATE_FLAG_ASYNCHRONOUS                      = 1u << 3u,

    PSO_CREATE_FLAG_LAST = PSO_CREATE_FLAG_ASYNCHRONOUS
};
DEFINE_FLAG_ENUM_OPERATORS(PSO_CREATE_FLAGS)

enum PIPELINE_STATE_STATUS : uint8_t
{
    /// Initial state.
    PIPELINE_STATE_STATUS_UNINITIALIZED = 0,

    /// The pipeline state is being compiled.
    PIPELINE_STATE_STATUS_COMPILING,

    /// The pipeline state has been successfully compiled
    /// and is ready to be used.
    PIPELINE_STATE_STATUS_READY,

    /// The pipeline state compilation has failed.
    PIPELINE_STATE_STATUS_FAILED
};


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

/// Describes shader variable
struct ShaderResourceVariableDesc
{
    /// Shader variable name
    const char*                   Name         = nullptr;

    /// Shader stages this resources variable applies to.

    /// If more than one shader stage is specified, the variable will be shared between these stages.
    /// Shader stages used by different variables with the same name must not overlap.
    SHADER_TYPE                   ShaderStages = SHADER_TYPE_UNKNOWN;

    /// Shader variable type. See SHADER_RESOURCE_VARIABLE_TYPE for a list of allowed types
    SHADER_RESOURCE_VARIABLE_TYPE Type         = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

    SHADER_VARIABLE_FLAGS         Flags        = SHADER_VARIABLE_FLAG_NONE;

    constexpr ShaderResourceVariableDesc() noexcept {}

    constexpr ShaderResourceVariableDesc(SHADER_TYPE                   _ShaderStages,
                                         const char*                   _Name,
                                         SHADER_RESOURCE_VARIABLE_TYPE _Type,
                                         SHADER_VARIABLE_FLAGS         _Flags = SHADER_VARIABLE_FLAG_NONE) noexcept :
        Name        {_Name        },
        ShaderStages{_ShaderStages},
        Type        {_Type        },
        Flags       {_Flags       }
    {}

    bool operator==(const ShaderResourceVariableDesc& _rhs) const noexcept
    {
        return ShaderStages == _rhs.ShaderStages &&
               Type         == _rhs.Type         &&
               Flags        == _rhs.Flags        &&
               SafeStrEqual(Name, _rhs.Name);
    }

    bool operator!=(const ShaderResourceVariableDesc& _rhs) const noexcept
    {
        return !(*this == _rhs);
    }
};

/// Pipeline layout description
struct PipelineResourceLayoutDesc
{
    /// Default shader resource variable type.

    /// This type will be used if shader variable description is not found in the `Variables` array
    /// or if `Variables == nullptr`
    SHADER_RESOURCE_VARIABLE_TYPE       DefaultVariableType  = SHADER_RESOURCE_VARIABLE_TYPE_STATIC;

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
    SHADER_TYPE                         DefaultVariableMergeStages = SHADER_TYPE_UNKNOWN;

    /// Number of elements in Variables array
    uint32_t                            NumVariables         = 0;

    /// Array of shader resource variable descriptions

    /// There may be multiple variables with the same name that use different shader stages,
    /// but the stages must not overlap.
    const ShaderResourceVariableDesc*   Variables            = nullptr;

    /// Number of immutable samplers in ImmutableSamplers array
    uint32_t                            NumImmutableSamplers = 0;

    /// Array of immutable sampler descriptions
    const ImmutableSamplerDesc*         ImmutableSamplers    = nullptr;

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
            for (uint32_t i = 0; i < Desc1.NumVariables; ++i)
                if (Desc1.Variables[i] != Desc2.Variables[i])
                    return false;
        }

        if (!IgnoreSamplers)
        {
            for (uint32_t i = 0; i < Desc1.NumImmutableSamplers; ++i)
                if (Desc1.ImmutableSamplers[i] != Desc2.ImmutableSamplers[i])
                    return false;
        }

        return true;
    }

    /// Comparison operator tests if two structures are equivalent
    bool operator==(const PipelineResourceLayoutDesc& _rhs) const noexcept
    {
        return IsEqual(*this, _rhs);
    }

    bool operator!=(const PipelineResourceLayoutDesc& _rhs) const noexcept
    {
        return !(*this == _rhs);
    }
};

struct GraphicsPipelineDesc
{
    BlendStateDesc BlendDesc;

    /// 32-bit sample mask that determines which samples get updated
    /// in all the active render targets. A sample mask is always applied;
    /// it is independent of whether multisampling is enabled, and does not
    /// depend on whether an application uses multisample render targets.
    uint32_t SampleMask = 0xFFFFFFFF;

    /// Rasterizer state description.
    RasterizerStateDesc RasterizerDesc;

    /// Depth-stencil state description.
    DepthStencilStateDesc DepthStencilDesc;

    /// Input layout, ignored in a mesh pipeline.
    InputLayoutDesc InputLayout;

    /// Primitive topology type, ignored in a mesh pipeline.
    PRIMITIVE_TOPOLOGY PrimitiveTopology = PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

    /// The number of viewports used by this pipeline
    uint8_t NumViewports           = 1;

    /// The number of render targets in the RTVFormats array.

    /// Must be 0 when `pRenderPass` is not `null`.
    uint8_t NumRenderTargets       = 0;

    /// When `pRenderPass` is not `null`, the subpass
    /// index within the render pass.
    /// When `pRenderPass` is `null`, this member must be 0.
    uint8_t SubpassIndex           = 0;

    /// Shading rate flags that specify which type of the shading rate will be used with this pipeline.
    PIPELINE_SHADING_RATE_FLAGS ShadingRateFlags = PIPELINE_SHADING_RATE_FLAG_NONE;

    /// Render target formats.

    /// All formats must be TEX_FORMAT_UNKNOWN when `pRenderPass` is not `null`.
    TEXTURE_FORMAT RTVFormats[MAX_RENDER_TARGETS] = {};

    /// Depth-stencil format.

    /// Must be TEX_FORMAT_UNKNOWN when `pRenderPass` is not `null`.
    TEXTURE_FORMAT DSVFormat     = TEX_FORMAT_UNKNOWN;

    /// Indicates that the pipeline will be used with read-only depth-stencil buffer.

    /// Must be `false` when `pRenderPass` is not `null`.
    bool ReadOnlyDSV             = false;

    /// Multisampling parameters.
    SampleDesc SmplDesc;

    // TODO: Add Render Passes

    /// Pointer to the render pass object.

    /// When non-null render pass is specified, `NumRenderTargets` must be 0,
    /// and all RTV formats as well as DSV format must be TEX_FORMAT_UNKNOWN.
    // IRenderPass* pRenderPass     = nullptr);

    /// Node mask.
    uint32_t NodeMask = 0;

    bool operator==(const GraphicsPipelineDesc& _rhs) const noexcept
    {
        if (!(BlendDesc         == _rhs.BlendDesc         &&
              SampleMask        == _rhs.SampleMask        &&
              RasterizerDesc    == _rhs.RasterizerDesc    &&
              DepthStencilDesc  == _rhs.DepthStencilDesc  &&
              InputLayout       == _rhs.InputLayout       &&
              PrimitiveTopology == _rhs.PrimitiveTopology &&
              NumViewports      == _rhs.NumViewports      &&
              NumRenderTargets  == _rhs.NumRenderTargets  &&
              SubpassIndex      == _rhs.SubpassIndex      &&
              ShadingRateFlags  == _rhs.ShadingRateFlags  &&
              DSVFormat         == _rhs.DSVFormat         &&
              ReadOnlyDSV       == _rhs.ReadOnlyDSV    &&
              SmplDesc          == _rhs.SmplDesc          &&
              NodeMask          == _rhs.NodeMask))
            return false;

        for (uint32_t i = 0; i < NumRenderTargets; ++i)
        {
            if (RTVFormats[i] != _rhs.RTVFormats[i])
                return false;
        }

        // if ((pRenderPass != nullptr) != (_rhs.pRenderPass != nullptr))
        //     return false;
        //
        // if (pRenderPass != nullptr)
        // {
        //     if (!(pRenderPass->GetDesc() == _rhs.pRenderPass->GetDesc()))
        //         return false;
        // }

        return true;
    }

    bool operator!=(const GraphicsPipelineDesc& _rhs) const noexcept
    {
        return !(*this == _rhs);
    }

};

struct PipelineStateDesc : public DeviceObjectAttribs
{
    PIPELINE_TYPE PipelineType = PIPELINE_TYPE_GRAPHICS;

    /// Shader resource binding allocation granularity

    /// This member defines allocation granularity for internal resources required by the shader resource
    /// binding object instances.
    /// Has no effect if the PSO is created with explicit pipeline resource signature(s).
    uint32_t SRBAllocationGranularity = 1u;


    /// Defines which immediate contexts are allowed to execute commands that use this pipeline state.

    /// When `ImmediateContextMask` contains a bit at position n, the pipeline state may be
    /// used in the immediate context with index n directly (see DeviceContextDesc::ContextId).
    /// It may also be used in a command list recorded by a deferred context that will be executed
    /// through that immediate context.
    ///
    /// \remarks    Only specify those bits that will indicate the immediate contexts where the PSO
    ///             will actually be used. Do not set unnecessary bits as this will result in extra overhead.
    uint64_t ImmediateContextMask = 1u;

    PipelineResourceLayoutDesc ResourceLayout;

    constexpr PipelineStateDesc() noexcept {}

    explicit constexpr PipelineStateDesc(const char*   _name,
                                         PIPELINE_TYPE _type = PipelineStateDesc{}.PipelineType)
        : DeviceObjectAttribs{_name}
        , PipelineType       {_type}
    {}

    /// Tests if two pipeline state descriptions are equal.

    /// \param [in] _rhs - reference to the structure to compare with.
    ///
    /// \return     true if all members of the two structures *except for the Name* are equal,
    ///             and false otherwise.
    ///
    /// \note   The operator ignores the Name field as it is used for debug purposes and
    ///         doesn't affect the pipeline state properties.
    bool operator==(const PipelineStateDesc& _rhs) const noexcept
    {
        // Ignore Name. This is consistent with the hasher (HashCombiner<HasherType, PipelineStateDesc>).
        return PipelineType             == _rhs.PipelineType             &&
               SRBAllocationGranularity == _rhs.SRBAllocationGranularity &&
               ImmediateContextMask     == _rhs.ImmediateContextMask     &&
               ResourceLayout           == _rhs.ResourceLayout;
    }
    bool operator!=(const PipelineStateDesc& _rhs) const noexcept
    {
        return !(*this == _rhs);
    }

    bool IsAnyGraphicsPipeline() const { return PipelineType == PIPELINE_TYPE_GRAPHICS || PipelineType == PIPELINE_TYPE_MESH; }
    bool IsComputePipeline()     const { return PipelineType == PIPELINE_TYPE_COMPUTE; }
    bool IsRayTracingPipeline()  const { return PipelineType == PIPELINE_TYPE_RAY_TRACING; }
    bool IsTilePipeline()        const { return PipelineType == PIPELINE_TYPE_TILE; }
};

struct PipelineStateCreateInfo
{
    PipelineStateDesc PSODesc;
    PSO_CREATE_FLAGS  Flags = PSO_CREATE_FLAG_NONE;

    /// The number of elements in `ppResourceSignatures` array.
    uint32_t ResourceSignaturesCount = 0;

    /// An array of `ResourceSignaturesCount` shader resource signatures that
    /// define the layout of shader resources in this pipeline state object.
    /// See Diligent::IPipelineResourceSignature.
    ///
    /// When this member is null, the pipeline resource layout will be defined
    /// by `PSODesc.ResourceLayout` member. In this case the PSO will implicitly
    /// create a resource signature that can be queried through IPipelineState::GetResourceSignature()
    /// method.
    /// When `ppResourceSignatures` is not null, `PSODesc.ResourceLayout` is ignored and
    /// should be in it default state.
    IPipelineResourceSignature** ppResourceSignatures = nullptr;

    /// Optional pipeline state cache that is used to accelerate
    /// PSO creation. If `PSODesc.Name` is found in the cache, the cache
    /// data is used to create the PSO. Otherwise, the PSO
    /// is added to the cache.
    IPipelineStateCache* pPSOCache     = nullptr;

    /// For internal use only. Must always be `null`.
    void*                pInternalData = nullptr;

    constexpr PipelineStateCreateInfo() noexcept {}

    constexpr PipelineStateCreateInfo(const char* _name, PIPELINE_TYPE _type) :
        PSODesc{_name, _type}
    {}

    bool operator==(const PipelineStateCreateInfo& _rhs) const noexcept
    {
        if (PSODesc                 != _rhs.PSODesc ||
            Flags                   != _rhs.Flags   ||
            ResourceSignaturesCount != _rhs.ResourceSignaturesCount)
            return false;

        if (ppResourceSignatures != _rhs.ppResourceSignatures)
        {
            if ((ppResourceSignatures == nullptr) != (_rhs.ppResourceSignatures == nullptr))
                return false;

            for (uint32_t i = 0; i < ResourceSignaturesCount; ++i)
            {
                const auto* pSign0 = ppResourceSignatures[i];
                const auto* pSign1 = _rhs.ppResourceSignatures[i];
                if (pSign0 == pSign1)
                    continue;

                if ((pSign0 == nullptr) != (pSign1 == nullptr))
                    return false;

                if (!pSign0->IsCompatibleWith(pSign1))
                    return false;
            }
        }

        // Ignore PSO cache and pInternalData

        return true;
    }

    bool operator!=(const PipelineStateCreateInfo& _rhs) const noexcept
    {
        return !(*this == _rhs);
    }
};

struct GraphicsPipelineStateCreateInfo : public PipelineStateCreateInfo
{
    GraphicsPipelineDesc GraphicsPipeline;

    IShader* pVS = nullptr;
    IShader* pPS = nullptr;
    IShader* pDS = nullptr;
    IShader* pHS = nullptr;
    IShader* pGS = nullptr;
    /// Amplification shader to be used with the pipeline
    IShader* pAS = nullptr;
    /// Mesh shader to be used with the pipeline
    IShader* pMS = nullptr;

    constexpr GraphicsPipelineStateCreateInfo() noexcept
    {
        PSODesc.PipelineType = PIPELINE_TYPE_GRAPHICS;
    }

    explicit constexpr GraphicsPipelineStateCreateInfo(const char* _name) :
        PipelineStateCreateInfo{_name, PIPELINE_TYPE_GRAPHICS}
    {}

    bool operator==(const GraphicsPipelineStateCreateInfo& _rhs) const noexcept
    {
        if (static_cast<const PipelineStateCreateInfo&>(*this) != static_cast<const PipelineStateCreateInfo&>(_rhs))
            return false;

        if (GraphicsPipeline != _rhs.GraphicsPipeline)
            return false;

        return (pVS == _rhs.pVS &&
                pPS == _rhs.pPS &&
                pDS == _rhs.pDS &&
                pHS == _rhs.pHS &&
                pGS == _rhs.pGS &&
                pAS == _rhs.pAS &&
                pMS == _rhs.pMS);
    }
    bool operator!=(const GraphicsPipelineStateCreateInfo& _rhs) const noexcept
    {
        return !(*this == _rhs);
    }
};

/// Compute pipeline state description.
struct ComputePipelineStateCreateInfo : public PipelineStateCreateInfo
{
    /// Compute shader to be used with the pipeline
    IShader* pCS = nullptr;

    ComputePipelineStateCreateInfo() noexcept
    {
        PSODesc.PipelineType = PIPELINE_TYPE_COMPUTE;
    }

    explicit constexpr ComputePipelineStateCreateInfo(const char* _name) :
        PipelineStateCreateInfo{_name, PIPELINE_TYPE_COMPUTE}
    {}

    bool operator==(const ComputePipelineStateCreateInfo& _rhs) const noexcept
    {
        if (static_cast<const PipelineStateCreateInfo&>(*this) != static_cast<const PipelineStateCreateInfo&>(_rhs))
            return false;

        return pCS == _rhs.pCS;
    }

    bool operator!=(const ComputePipelineStateCreateInfo& _rhs) const noexcept
    {
        return !(*this == _rhs);
    }
};

struct IPipelineState : IDeviceObject
{
    virtual const PipelineStateDesc& GetDesc() const override = 0;

    virtual const GraphicsPipelineDesc& GetGraphicsPipelineDesc() const = 0;

    /// Binds resources for all shaders in the pipeline state.

    /// \param [in] _shaderStages     - Flags that specify shader stages, for which resources will be bound.
    ///                                Any combination of Diligent::SHADER_TYPE may be used.
    /// \param [in] _pResourceMapping - Pointer to the resource mapping interface.
    /// \param [in] _flags            - Additional flags. See Diligent::BIND_SHADER_RESOURCES_FLAGS.
    ///
    /// This method is only allowed for pipelines that use implicit resource signature
    /// (e.g. shader resources are defined through ResourceLayout member of the pipeline desc).
    /// For pipelines that use explicit resource signatures, use
    /// IPipelineResourceSignature::BindStaticResources() method.
    virtual void BindStaticResources(SHADER_TYPE                 _shaderStages,
                                     IResourceMapping*           _pResourceMapping,
                                     BIND_SHADER_RESOURCES_FLAGS _flags) = 0;

    /// Returns the number of static shader resource variables.

    /// \param [in] _shaderType - Type of the shader.
    ///
    /// Only static variables (that can be accessed directly through the PSO) are counted.
    /// Mutable and dynamic variables are accessed through Shader Resource Binding object.
    ///
    /// This method is only allowed for pipelines that use implicit resource signature
    /// (e.g. shader resources are defined through ResourceLayout member of the pipeline desc).
    /// For pipelines that use explicit resource signatures, use
    /// IPipelineResourceSignature::GetStaticVariableCount() method.
    virtual uint32_t GetStaticVariableCount(SHADER_TYPE _shaderType) const = 0;


    /// Returns static shader resource variable by its index.

    /// \param [in] _shaderType - The type of the shader to look up the variable.
    ///                          Must be one of Diligent::SHADER_TYPE.
    /// \param [in] _index      - Shader variable index. The index must be between
    ///                          0 and the total number of variables returned by
    ///                          GetStaticVariableCount().
    ///
    /// Only static shader resource variables can be accessed through this method.
    /// Mutable and dynamic variables are accessed through Shader Resource
    /// Binding object.
    ///
    /// This method is only allowed for pipelines that use implicit resource signature
    /// (e.g. shader resources are defined through ResourceLayout member of the pipeline desc).
    /// For pipelines that use explicit resource signatures, use
    /// IPipelineResourceSignature::GetStaticVariableByIndex() method.
    virtual IShaderResourceVariable* GetStaticVariableByIndex(SHADER_TYPE _shaderType,
                                                              uint32_t    _index) = 0;


    /// Creates a shader resource binding object.

    /// \param [out] _ppShaderResourceBinding - Memory location where pointer to the new shader resource
    ///                                        binding object is written.
    /// \param [in] _initStaticResources      - If set to true, the method will initialize static resources in
    ///                                        the created object, which has the exact same effect as calling
    ///                                        IPipelineState::InitializeStaticSRBResources().
    ///
    /// This method is only allowed for pipelines that use implicit resource signature
    /// (e.g. shader resources are defined through ResourceLayout member of the pipeline desc).
    /// For pipelines that use explicit resource signatures, use
    /// IPipelineResourceSignature::CreateShaderResourceBinding() method.
    virtual void CreateShaderResourceBinding(IShaderResourceBinding** _ppShaderResourceBinding,
                                             bool                     _initStaticResources = false) = 0;

    /// Initializes static resources in the shader binding object.

    /// If static shader resources were not initialized when the SRB was created,
    /// this method must be called to initialize them before the SRB can be used.
    /// The method should be called after all static variables have been initialized
    /// in the PSO.
    ///
    /// \param [in] _pShaderResourceBinding - Shader resource binding object to initialize.
    ///                                      The pipeline state must be compatible
    ///                                      with the shader resource binding object.
    ///
    /// \note   If static resources have already been initialized in the SRB and the method
    ///         is called again, it will have no effect and a warning message will be displayed.
    ///
    /// This method is only allowed for pipelines that use implicit resource signature
    /// (e.g. shader resources are defined through ResourceLayout member of the pipeline desc).
    /// For pipelines that use explicit resource signatures, use
    /// IPipelineResourceSignature::InitializeStaticSRBResources() method.
    virtual void InitializeStaticSRBResources(IShaderResourceBinding* _pShaderResourceBinding) = 0;


    /// Copies static resource bindings to the destination pipeline.

    /// \param [in] _pDstPipeline - Destination pipeline state.
    ///
    /// \note   Destination pipeline state must be compatible with this pipeline.
    ///
    /// This method is only allowed for pipelines that use implicit resource signature
    /// (e.g. shader resources are defined through ResourceLayout member of the pipeline desc).
    /// For pipelines that use explicit resource signatures, use
    /// IPipelineResourceSignature::CopyStaticResources() method.
    virtual void CopyStaticResources(IPipelineState* _pDstPipeline) const = 0;


    /// Checks if this pipeline state object is compatible with another PSO

    /// If two pipeline state objects are compatible, they can use shader resource binding
    /// objects interchangeably, i.e. SRBs created by one PSO can be committed
    /// when another PSO is bound.
    ///
    /// \param [in] _pPSO - Pointer to the pipeline state object to check compatibility with.
    /// \return     true if this PSO is compatible with pPSO. false otherwise.
    ///
    /// The function only checks that shader resource layouts are compatible, but
    /// does not check if resource types match. For instance, if a pixel shader in one PSO
    /// uses a texture at slot 0, and a pixel shader in another PSO uses texture array at slot 0,
    /// the pipelines will be compatible. However, if you try to use SRB object from the first pipeline
    /// to commit resources for the second pipeline, a runtime error will occur.\n
    /// The function only checks compatibility of shader resource layouts. It does not take
    /// into account vertex shader input layout, number of outputs, etc.
    ///
    /// **Technical details**
    ///
    /// PSOs may be partially compatible when some, but not all pipeline resource signatures are compatible.
    /// In Vulkan backend, switching PSOs that are partially compatible may increase performance
    /// as shader resource bindings (that map to descriptor sets) from compatible signatures may be preserved.
    /// In Direct3D12 backend, only switching between fully compatible PSOs preserves shader resource bindings,
    /// while switching partially compatible PSOs still requires re-binding all resource bindings from all signatures.
    /// In other backends the behavior is emulated. Usually, the bindings from the first N compatible resource signatures
    /// may be preserved.
    virtual bool IsCompatibleWith(const IPipelineState* _pPSO) const = 0;


    /// Returns pipeline resource signature at the given index.

    /// \param [in] _index - Index of the resource signature, same as BindingIndex in PipelineResourceSignatureDesc.
    /// \return     A pointer to the pipeline resource signature interface.
    virtual IPipelineResourceSignature* GetResourceSignature(uint32_t _index) const = 0;


    /// Returns the pipeline state status, see Diligent::PIPELINE_STATE_STATUS.

    /// \param [in] _waitForCompletion - If true, the method will wait until the pipeline state is compiled.
    ///                                 If false, the method will return the pipeline state status without waiting.
    ///                                 This parameter is ignored if the pipeline state was compiled synchronously.
    /// \return     The pipeline state status
    virtual PIPELINE_STATE_STATUS GetStatus(bool _waitForCompletion = false) = 0;
};

} // namespace Blainn