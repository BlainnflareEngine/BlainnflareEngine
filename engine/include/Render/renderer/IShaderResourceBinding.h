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
    ///                            Any combination of Diligent::SHADER_TYPE may be used.
    /// \param [in] _pResMapping  - Shader resource mapping where required resources will be looked up.
    /// \param [in] _flags        - Additional flags. See Diligent::BIND_SHADER_RESOURCES_FLAGS.
    virtual void BindResources(SHADER_TYPE                _shaderStages,
                               IResourceMapping*          _pResMapping,
                               BIND_SHADER_RESOURCE_FLAGS _flags) = 0;
};
}
