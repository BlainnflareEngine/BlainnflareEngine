#pragma once

#include "Render/DXHelpers.h"

namespace Blainnflare
{
    struct BlainnShaderDesc
    {

    };

    class Shader
    {
    public:
        enum class EShaderType : UINT
        {
            CascadedShadowsVS = 0u,
            CascadedShadowsGS,

            DeferredGeometryVS,
            DeferredGeometryPS,

            DeferredDirVS,
            DeferredDirPS,
            DeferredLightVolumesVS,
            DeferredPointPS,
            DeferredSpotPS,

            NumShaders = 9U
        };

    public:
        explicit Shader() = default;
        ~Shader() noexcept = default;
    
        void AddDefines();
    
    private:
        BlainnShaderDesc m_shaderDescriptor;
    };
}