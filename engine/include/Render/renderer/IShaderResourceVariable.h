//
// Created by WhoLeb on 27-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once
#include <cstdint>

#include "IShader.h"

namespace Blainn
{
enum SHADER_RESOURCE_VARIABLE_TYPE : uint8_t
{
    /// Shader resource bound to the variable is the same for all SRB instances.
    /// It must be set *once* directly through Pipeline State object.
    SHADER_RESOURCE_VARIABLE_TYPE_STATIC = 0,

    /// Shader resource bound to the variable is specific to the shader resource binding
    /// instance (see IShaderResourceBinding). It must be set *once* through
    /// IShaderResourceBinding interface. It cannot be set through IPipelineState
    /// interface and cannot be changed once bound.
    SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE,

    /// Shader variable binding is dynamic. It can be set multiple times for every instance of shader resource
    /// binding (see IShaderResourceBinding). It cannot be set through IPipelineState interface.
    SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC,

    /// Total number of shader variable types
    SHADER_RESOURCE_VARIABLE_TYPE_NUM_TYPES
};

enum SHADER_RESOURCE_VARIABLE_TYPE_FLAGS : uint32_t
{
    /// No flags
    SHADER_RESOURCE_VARIABLE_TYPE_FLAG_NONE    = 0x00,

    /// Static variable type flag
    SHADER_RESOURCE_VARIABLE_TYPE_FLAG_STATIC  = (0x01 << SHADER_RESOURCE_VARIABLE_TYPE_STATIC),

    /// Mutable variable type flag
    SHADER_RESOURCE_VARIABLE_TYPE_FLAG_MUTABLE = (0x01 << SHADER_RESOURCE_VARIABLE_TYPE_MUTABLE),

    /// Dynamic variable type flag
    SHADER_RESOURCE_VARIABLE_TYPE_FLAG_DYNAMIC = (0x01 << SHADER_RESOURCE_VARIABLE_TYPE_DYNAMIC),

    /// Mutable and dynamic variable type flags
    SHADER_RESOURCE_VARIABLE_TYPE_FLAG_MUT_DYN =
        SHADER_RESOURCE_VARIABLE_TYPE_FLAG_MUTABLE |
        SHADER_RESOURCE_VARIABLE_TYPE_FLAG_DYNAMIC,

    /// All variable type flags
    SHADER_RESOURCE_VARIABLE_TYPE_FLAG_ALL =
        SHADER_RESOURCE_VARIABLE_TYPE_FLAG_STATIC  |
        SHADER_RESOURCE_VARIABLE_TYPE_FLAG_MUTABLE |
        SHADER_RESOURCE_VARIABLE_TYPE_FLAG_DYNAMIC
};
DEFINE_FLAG_ENUM_OPERATORS(SHADER_RESOURCE_VARIABLE_TYPE_FLAGS);


enum BIND_SHADER_RESOURCES_FLAGS : uint32_t
{
    /// Indicates that static shader variable bindings are to be updated.
    BIND_SHADER_RESOURCES_UPDATE_STATIC = SHADER_RESOURCE_VARIABLE_TYPE_FLAG_STATIC,

    /// Indicates that mutable shader variable bindings are to be updated.
    BIND_SHADER_RESOURCES_UPDATE_MUTABLE = SHADER_RESOURCE_VARIABLE_TYPE_FLAG_MUTABLE,

    /// Indicates that dynamic shader variable bindings are to be updated.
    BIND_SHADER_RESOURCES_UPDATE_DYNAMIC = SHADER_RESOURCE_VARIABLE_TYPE_FLAG_DYNAMIC,

    /// Indicates that all shader variable types (static, mutable and dynamic) are to be updated.
    /// \note If none of BIND_SHADER_RESOURCES_UPDATE_STATIC, BIND_SHADER_RESOURCES_UPDATE_MUTABLE,
    ///       and BIND_SHADER_RESOURCES_UPDATE_DYNAMIC flags are set, all variable types are updated
    ///       as if BIND_SHADER_RESOURCES_UPDATE_ALL was specified.
    BIND_SHADER_RESOURCES_UPDATE_ALL = SHADER_RESOURCE_VARIABLE_TYPE_FLAG_ALL,

    /// If this flag is specified, all existing bindings will be preserved and
    /// only unresolved ones will be updated.
    /// If this flag is not specified, every shader variable will be
    /// updated if the mapping contains corresponding resource.
    BIND_SHADER_RESOURCES_KEEP_EXISTING = 0x08,

    /// If this flag is specified, all shader bindings are expected
    /// to be resolved after the call. If this is not the case, debug message
    /// will be displayed.
    ///
    /// \note Only these variables are verified that are being updated by setting
    ///       BIND_SHADER_RESOURCES_UPDATE_STATIC, BIND_SHADER_RESOURCES_UPDATE_MUTABLE, and
    ///       BIND_SHADER_RESOURCES_UPDATE_DYNAMIC flags.
    BIND_SHADER_RESOURCES_VERIFY_ALL_RESOLVED = 0x10,

    /// Allow overwriting static and mutable variables, see SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE.
    BIND_SHADER_RESOURCES_ALLOW_OVERWRITE = 0x20
};
DEFINE_FLAG_ENUM_OPERATORS(BIND_SHADER_RESOURCES_FLAGS);


enum SET_SHADER_RESOURCE_FLAGS : uint32_t
{
    /// No flags.
    SET_SHADER_RESOURCE_FLAG_NONE = 0,

    /// Allow overwriting static and mutable variable bindings.
    ///
    /// By default, static and mutable variables can't be changed once
    /// initialized to a non-null resource. This flag is required
    /// to explicitly allow overwriting the binding.
    ///
    /// Overwriting static variables does not require synchronization
    /// with GPU and does not have effect on shader resource binding
    /// objects already created from the pipeline state or resource signature.
    ///
    /// When overwriting a mutable variable binding in Direct3D12 and Vulkan,
    /// an application must ensure that the GPU is not accessing the SRB.
    /// This can be achieved using synchronization tools such as fences.
    /// Synchronization with GPU is not required in OpenGL, Direct3D11,
    /// and Metal backends.
    SET_SHADER_RESOURCE_FLAG_ALLOW_OVERWRITE = 1u << 0
};
DEFINE_FLAG_ENUM_OPERATORS(SET_SHADER_RESOURCE_FLAGS);

struct IShaderResourceVariable
{
    /// Binds resource to the variable

    /// The method performs run-time correctness checks.
    /// For instance, shader resource view cannot
    /// be assigned to a constant buffer variable.
    virtual void Set(IDeviceObject*            _pObject,
                     SET_SHADER_RESOURCE_FLAGS _flags = SET_SHADER_RESOURCE_FLAG_NONE) = 0;


    /// Binds resource array to the variable

    /// \param [in] _ppObjects    - a pointer to the array of objects.
    /// \param [in] _firstElement - first array element to set.
    /// \param [in] _numElements  - the number of objects in ppObjects array.
    /// \param [in] _flags        - flags, see SET_SHADER_RESOURCE_FLAGS.
    ///
    /// The method performs run-time correctness checks.
    /// For instance, shader resource view cannot
    /// be assigned to a constant buffer variable.
    ///
    virtual void SetArray(IDeviceObject*            _ppObjects   ,
                          uint32_t                  _firstElement,
                          uint32_t                  _numElements ,
                          SET_SHADER_RESOURCE_FLAGS _flags = SET_SHADER_RESOURCE_FLAG_NONE) = 0;


    /// Binds the specified constant buffer range to the variable

    /// \param [in] _pObject    - pointer to the buffer object.
    /// \param [in] _offset     - offset, in bytes, to the start of the buffer range to bind.
    /// \param [in] _size       - size, in bytes, of the buffer range to bind.
    /// \param [in] _arrayIndex - for array variables, index of the array element.
    /// \param [in] _flags      - flags, see SET_SHADER_RESOURCE_FLAGS.
    ///
    /// This method is only allowed for constant buffers. If dynamic offset is further set
    /// by SetBufferOffset() method, it is added to the base offset set by this method.
    ///
    /// The method resets dynamic offset previously set for this variable to zero.
    ///
    /// \warning The Offset must be an integer multiple of ConstantBufferOffsetAlignment member
    ///          specified by the device limits (see DeviceLimits).
    virtual void SetBufferRange(IDeviceObject*            _pObject    ,
                                uint64_t                  _offset     ,
                                uint64_t                  _size       ,
                                uint32_t                  _arrayIndex = 0,
                                SET_SHADER_RESOURCE_FLAGS _flags      = SET_SHADER_RESOURCE_FLAG_NONE) = 0;


    /// Sets the constant or structured buffer dynamic offset

    /// \param [in] _offset     - additional offset, in bytes, that is added to the base offset (see remarks).
    ///                          Only 32-bit offsets are supported.
    /// \param [in] _arrayIndex - for array variables, index of the array element.
    ///
    /// This method is only allowed for constant or structured buffer variables that
    /// were not created with SHADER_VARIABLE_FLAG_NO_DYNAMIC_BUFFERS or
    /// PIPELINE_RESOURCE_FLAG_NO_DYNAMIC_BUFFERS flags. The method is also not
    /// allowed for static resource variables.
    ///
    /// \note   The Offset must be an integer multiple of ConstantBufferOffsetAlignment member
    ///         when setting the offset for a constant buffer, or StructuredBufferOffsetAlignment when
    ///         setting the offset for a structured buffer, as specified by device limits
    ///         (see DeviceLimits).
    ///
    /// For constant buffers, the offset is added to the offset that was previously set
    /// by SetBufferRange() method (if any). For structured buffers, the offset is added
    /// to the base offset specified by the buffer view.
    ///
    /// Changing the buffer offset does not require committing the SRB.
    /// From the engine point of view, buffers with dynamic offsets are treated similar to dynamic
    /// buffers, and thus affected by DRAW_FLAG_DYNAMIC_RESOURCE_BUFFERS_INTACT flag.
    virtual void SetBufferOffset(uint32_t _offset,
                                 uint32_t _arrayIndex = 0) = 0;

    /// For inline constant variables, sets the constant values

    /// Inline constant variables are defined using SHADER_RESOURCE_VARIABLE_FLAG_INLINE_CONSTANTS
    /// or PIPELINE_RESOURCE_FLAG_INLINE_CONSTANTS flags.
    ///
    /// \param [in] _pConstants    - pointer to the array of 32-bit constant values.
    /// \param [in] _firstConstant - index of the first 32-bit constant to set.
    /// \param [in] _numConstants  - number of 32-bit constants to set.
    virtual void SetInlineConstants(const void* _pConstants   ,
                                    uint32_t    _firstConstant,
                                    uint32_t    _numConstants ) = 0;


    /// Returns the shader resource variable type
    virtual SHADER_RESOURCE_VARIABLE_TYPE GetType() const = 0;

    /// Returns shader resource description. See ShaderResourceDesc.
    virtual void GetResourceIndex(ShaderResourceDesc& _resourceDesc) const = 0;

    /// Returns the variable index that can be used to access the variable.
    virtual uint32_t GetIndex() const = 0;

    /// Returns a pointer to the resource that is bound to this variable.

    /// \param [in] _arrayIndex - Resource array index. Must be 0 for
    ///                          non-array variables.
    virtual IDeviceObject* Get(uint32_t _arrayIndex = 0) const = 0;
};
}
