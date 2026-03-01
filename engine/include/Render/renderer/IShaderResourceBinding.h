//
// Created by WhoLeb on 27-Feb-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once

#include "IResourceMapping.h"
#include "IShaderResourceVariable.h"
#include "Render/GraphicsTypes.h"

namespace Blainn
{
struct IPipelineState;
struct IPipelineResourceSignature;

struct IShaderResourceBinding
{
    /// Returns a pointer to the pipeline resource signature object that
    /// defines the layout of this shader resource binding object.
    virtual IPipelineResourceSignature* GetPipelineResourceSignature() const = 0;


    /// Binds SRB resources using the resource mapping

    /// \param [in] _shaderStages - Flags that specify shader stages, for which resources will be bound.
    ///                            Any combination of SHADER_TYPE may be used.
    /// \param [in] _pResMapping  - Shader resource mapping where required resources will be looked up.
    /// \param [in] _flags        - Additional flags. See BIND_SHADER_RESOURCES_FLAGS.
    virtual void BindResources(SHADER_TYPE                 _shaderStages,
                               IResourceMapping*           _pResMapping,
                               BIND_SHADER_RESOURCES_FLAGS _flags) = 0;

    /// Checks currently bound resources, see remarks.

    /// \param [in] _shaderStages - Flags that specify shader stages, for which to check resources.
    ///                            Any combination of SHADER_TYPE may be used.
    /// \param [in] _pResMapping  - Optional shader resource mapping where resources will be looked up.
    ///                            May be null.
    /// \param [in] _flags        - Additional flags, see remarks.
    ///
    /// \return     Variable type flags that did not pass the checks and thus may need to be updated.
    ///
    /// This method may be used to perform various checks of the currently bound resources:
    ///
    /// - BIND_SHADER_RESOURCES_UPDATE_MUTABLE and BIND_SHADER_RESOURCES_UPDATE_DYNAMIC flags
    ///   define which variable types to examine. Note that BIND_SHADER_RESOURCES_UPDATE_STATIC
    ///   has no effect as static resources are accessed through the PSO.
    ///
    /// - If BIND_SHADER_RESOURCES_KEEP_EXISTING flag is not set and pResMapping is not null,
    ///   the method will compare currently bound resources with the ones in the resource mapping.
    ///   If any mismatch is found, the method will return the types of the variables that
    ///   contain mismatching resources.
    ///   Note that the situation when non-null object is bound to the variable, but the resource
    ///   mapping does not contain an object corresponding to the variable name, does not count as
    ///   mismatch.
    ///
    /// - If BIND_SHADER_RESOURCES_VERIFY_ALL_RESOLVED flag is set, the method will check that
    ///   all resources of the specified variable types are bound and return the types of the variables
    ///   that are not bound.
    virtual SHADER_RESOURCE_VARIABLE_TYPE_FLAGS CheckResources(SHADER_TYPE                 _shaderStages,
                                                               IResourceMapping*           _pResMapping ,
                                                               BIND_SHADER_RESOURCES_FLAGS _flags       ) const = 0;

    /// Returns the variable by its name.

    /// \param [in] _shaderType - Type of the shader to look up the variable.
    ///                          Must be one of SHADER_TYPE.
    /// \param [in] _name       - Variable name.
    ///
    /// \note  This operation may potentially be expensive. If the variable will often be used, it is
    ///        recommended to store and reuse the pointer as it never changes.
    virtual IShaderResourceVariable* GetVariableByName(SHADER_TYPE _shaderType,
                                                       const char* _name) = 0;


    /// Returns the total variable count for the specific shader stage.

    /// \param [in] _shaderType - Type of the shader.
    /// \return Total number of variables in the shader stage.
    ///
    /// The method only counts mutable and dynamic variables that can be accessed through
    /// the Shader Resource Binding object. Static variables are accessed through the Shader
    /// object.
    virtual uint32_t GetVariableCount(SHADER_TYPE _shaderType) const = 0;

    /// Returns the variable by its index.

    /// \param [in] _shaderType - Type of the shader to look up the variable.
    ///                          Must be one of SHADER_TYPE.
    /// \param [in] _index      - Variable index. The index must be between 0 and the total number
    ///                          of variables in this shader stage as returned by
    ///                          IShaderResourceBinding::GetVariableCount().
    ///
    /// Only mutable and dynamic variables can be accessed through this method.
    /// Static variables are accessed through the Shader object.
    ///
    /// \note   This operation may potentially be expensive. If the variable will be used often, it is
    ///         recommended to store and reuse the pointer as it never changes.
    virtual IShaderResourceVariable* GetVariableByIndex(SHADER_TYPE _shaderType,
                                                        uint32_t    _index) = 0;

    /// Returns true if static resources have been initialized in this SRB.
    virtual bool StaticResourcesInitialized() const = 0;
};
}
