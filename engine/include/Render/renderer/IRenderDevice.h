//
// Created by WhoLeb on 21-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once

#include <cstdint>

#include "IBuffer.h"
#include "IShader.h"
#include "ITexture.h"
#include "Render/GraphicsTypes.h"
#include "Render/memory/IDataBlob.h"
#include "PipelineState.h"

namespace Blainn
{
struct IRenderDevice
{
    /// Creates a new buffer object.
    ///
    /// \param [in] _buffDesc  - Buffer description.
    /// \param [in] _pBuffData - Initial buffer data or nullptr if no data is provided.
    /// \param [out] _ppBuffer - Address where the buffer interface pointer will be written.
    ///
    /// Size of a uniform buffer (BIND_UNIFORM_BUFFER) must be multiple of 16.
    /// Stride of a formatted buffer is computed automatically when ElementByteStride is zero.
    virtual void CreateBuffer(const BufferDesc& _buffDesc,
                              const BufferData* _pBuffData,
                                    IBuffer**   _ppBuffer) = 0;

    /// Creates a new shader object.
    ///
    /// \param [in] _shaderCI          - Shader create info.
    /// \param [out] _ppShader         - Address where the shader interface pointer will be written.
    /// \param [out] _ppCompilerOutput - Address where compiler output blob pointer will be written.
    ///                                  If nullptr, compiler output is ignored.
    ///
    /// The output blob contains two null-terminated strings:
    /// compiler message and full preprocessed shader source.
    virtual void CreateShader(const ShaderCreateInfo& _shaderCI,
                                    IShader**         _ppShader,
                                    IDataBlob**       _ppCompilerOutput = nullptr) = 0;

    /// Creates a new texture object.
    ///
    /// \param [in] _texDesc   - Texture description.
    /// \param [in] _pData     - Initial texture data or nullptr if no data is provided.
    /// \param [out] _ppTexture - Address where the texture interface pointer will be written.
    ///
    /// To create all mip levels, set _texDesc.MipLevels to zero.
    /// Multisampled resources cannot be initialized with data at creation.
    /// If initial data is provided, number of subresources must exactly match the number
    /// of subresources in the texture (which is the number of mip levels times the number of array slices.
    /// For a 3D texture, this is just the number of mip levels).
    virtual void CreateTexture(const TextureDesc& _texDesc,
                               const TextureData* _pData,
                                     ITexture**   _ppTexture) = 0;

    /// Creates a new sampler object.
    ///
    /// \param [in] _samplerDesc - Sampler description.
    /// \param [out] _ppSampler  - Address where the sampler interface pointer will be written.
    virtual void CreateSampler(const SamplerDesc& _samplerDesc,
                                     ISampler**   _ppSampler) = 0;

    // TODO
    /// Creates a new resource mapping.
    ///
    /// \param [in] _resMappingCI - Resource mapping create info.
    /// \param [out] _ppMapping   - Address where the mapping interface pointer will be written.
    virtual void CreateResourceMapping(const ResourceMappingCreateInfo& _resMappingCI,
                                             IResourceMapping**         _ppMapping) = 0;

    /// Creates a new graphics pipeline state object.
    ///
    /// \param [in] _psoCI             - Graphics pipeline state create info.
    /// \param [out] _ppPipelineState  - Address where the pipeline state interface pointer will be written.
    virtual void CreateGraphicsPipelineState(const GraphicsPipelineStateCreateInfo& _psoCI,
                                                   IPipelineState**                 _ppPipelineState) = 0;

    /// Creates a new compute pipeline state object.
    ///
    /// \param [in] _psoCI            - Compute pipeline state create info.
    /// \param [out] _ppPipelineState - Address where the pipeline state interface pointer will be written.
    virtual void CreateComputePipelineState(const ComputePipelineStateCreateInfo& _psoCI,
                                                  IPipelineState**                _ppPipelineState) = 0;

    // TODO
    // /// Creates a new ray tracing pipeline state object.
    // ///
    // /// \param [in] _psoCI            - Ray tracing pipeline state create info.
    // /// \param [out] _ppPipelineState - Address where the pipeline state interface pointer will be written.
    // virtual void CreateRayTracingPipelineState(const RayTracingPipelineStateCI& _psoCI,
    //                                                  IPipelineState**           _ppPipelineState) = 0;

    // TODO
    // /// Creates a new tile pipeline state object.
    // ///
    // /// \param [in] _psoCI            - Tile pipeline state create info.
    // /// \param [out] _ppPipelineState - Address where the pipeline state interface pointer will be written.
    // virtual void CreateTilePipelineState(const TilePipelineStateCI& _psoCI,
    //                                            IPipelineState**     _ppPipelineState) = 0;

    /// Creates a new fence object.
    ///
    /// \param [in] _desc    - Fence description.
    /// \param [out] _ppFence - Address where the fence interface pointer will be written.
    virtual void CreateFence(const FenceDesc& _desc,
                                   IFence**   _ppFence) = 0;

    // TODO
    // /// Creates a new query object.
    // ///
    // /// \param [in] _desc    - Query description.
    // /// \param [out] _ppQuery - Address where the query interface pointer will be written.
    // virtual void CreateQuery(const QueryDesc& _desc,
    //                                IQuery**   _ppQuery) = 0;

    // TODO
    // /// Creates a render pass object.
    // ///
    // /// \param [in] _desc         - Render pass description.
    // /// \param [out] _ppRenderPass - Address where the render pass interface pointer will be written.
    // virtual void CreateRenderPass(const RenderPassDesc& _desc,
    //                                     IRenderPass**   _ppRenderPass) = 0;

    // TODO
    // /// Creates a framebuffer object.
    // ///
    // /// \param [in] _desc           - Framebuffer description.
    // /// \param [out] _ppFramebuffer - Address where the framebuffer interface pointer will be written.
    // virtual void CreateFramebuffer(const FramebufferDesc& _desc,
    //                                      IFramebuffer**   _ppFramebuffer) = 0;

    // TODO
    // /// Creates a bottom-level acceleration structure object (BLAS).
    // ///
    // /// \param [in] _desc   - BLAS description.
    // /// \param [out] _ppBLAS - Address where the BLAS interface pointer will be written.
    // virtual void CreateBLAS(const BottomLevelASDesc& _desc,
    //                               IBottomLevelAS**   _ppBLAS) = 0;

    // TODO
    // /// Creates a top-level acceleration structure object (TLAS).
    // ///
    // /// \param [in] _desc   - TLAS description.
    // /// \param [out] _ppTLAS - Address where the TLAS interface pointer will be written.
    // virtual void CreateTLAS(const TopLevelASDesc& _desc,
    //                               ITopLevelAS**   _ppTLAS) = 0;

    // TODO
    // /// Creates a shader binding table object (SBT).
    // ///
    // /// \param [in] _desc   - SBT description.
    // /// \param [out] _ppSBT - Address where the SBT interface pointer will be written.
    // virtual void CreateSBT(const ShaderBindingTableDesc& _desc,
    //                              IShaderBindingTable**   _ppSBT) = 0;

    /// Creates a pipeline resource signature object.
    ///
    /// \param [in] _desc        - Resource signature description.
    /// \param [out] _ppSignature - Address where the signature interface pointer will be written.
    virtual void CreatePipelineResourceSignature(const PipelineResourceSignatureDesc& _desc,
                                                       IPipelineResourceSignature**   _ppSignature) = 0;

    /// Creates a device memory object.
    ///
    /// \param [in] _createInfo - Device memory create info.
    /// \param [out] _ppMemory  - Address where the device memory interface pointer will be written.
    // virtual void CreateDeviceMemory(const DeviceMemoryCreateInfo& _createInfo,
    //                                       IDeviceMemory**         _ppMemory) = 0;

    // TODO
    /// Creates a pipeline state cache object.
    ///
    /// \param [in] _createInfo - Pipeline state cache create info.
    /// \param [out] _ppPSOCache - Address where the cache interface pointer will be written.
    ///
    /// On backends without PSO cache support, this may silently do nothing.
    virtual void CreatePipelineStateCache(const PipelineStateCacheCreateInfo& _createInfo,
                                                IPipelineStateCache**         _ppPSOCache) = 0;

    // TODO
    /// Creates a deferred context.
    ///
    /// \param [out] _ppContext - Address where the deferred context interface pointer will be written.
    ///
    /// \remarks Deferred contexts are not supported on OpenGL and WebGPU backends.
    // virtual void CreateDeferredContext(IDeviceContext** _ppContext) = 0;

    /// Returns device information.
    virtual const RenderDeviceInfo& GetDeviceInfo() const = 0;

    /// Returns graphics adapter information.
    virtual const GraphicsAdapterInfo& GetAdapterInfo() const = 0;

    /// Returns basic texture format information.
    ///
    /// \param [in] _textureFormat - Texture format to query.
    /// \return Const reference to the basic texture format information.
    virtual const TextureFormatInfo& GetTextureFormatInfo(TEXTURE_FORMAT _textureFormat) const = 0;

    /// Returns extended texture format information.
    ///
    /// \param [in] _textureFormat - Texture format to query.
    /// \return Const reference to the extended texture format information.
    virtual const TextureFormatInfoExt& GetTextureFormatInfoExt(TEXTURE_FORMAT _textureFormat) = 0;

    /// Returns sparse texture format information for format, dimension and sample count.
    virtual SparseTextureFormatInfo GetSparseTextureFormatInfo(TEXTURE_FORMAT     _textureFormat,
                                                               RESOURCE_DIMENSION _dimension,
                                                               uint32_t           _sampleCount) const = 0;

    /// Purges release queues and releases stale resources.
    ///
    /// \param [in] _forceRelease - Forces release of all objects. Use carefully and only
    ///                             when resources are guaranteed not to be in GPU use.
    virtual void ReleaseStaleResources(bool _forceRelease = false) = 0;

    /// Waits until all outstanding GPU operations are complete.
    virtual void IdleGPU() = 0;

    /// Returns the engine factory that created this device.
    ///
    /// Returned pointer is borrowed and must not be released by the caller.
    virtual IEngineFactory* GetEngineFactory() const = 0;

    // TODO
    /// Returns the shader compilation thread pool.
    ///
    /// Returned pointer is borrowed and must not be released by the caller.
    // virtual IThreadPool* GetShaderCompilationThreadPool() const = 0;

    /// Overloaded alias for CreateGraphicsPipelineState().
    void CreatePipelineState(const GraphicsPipelineStateCI& _ci, IPipelineState** _ppPipelineState)
    {
        CreateGraphicsPipelineState(_ci, _ppPipelineState);
    }

    /// Overloaded alias for CreateComputePipelineState().
    void CreatePipelineState(const ComputePipelineStateCI& _ci, IPipelineState** _ppPipelineState)
    {
        CreateComputePipelineState(_ci, _ppPipelineState);
    }

    // TODO
    // /// Overloaded alias for CreateRayTracingPipelineState().
    // void CreatePipelineState(const RayTracingPipelineStateCI& _ci, IPipelineState** _ppPipelineState)
    // {
    //     CreateRayTracingPipelineState(_ci, _ppPipelineState);
    // }
    //
    // /// Overloaded alias for CreateTilePipelineState().
    // void CreatePipelineState(const TilePipelineStateCI& _ci, IPipelineState** _ppPipelineState)
    // {
    //     CreateTilePipelineState(_ci, _ppPipelineState);
    // }
};
}
