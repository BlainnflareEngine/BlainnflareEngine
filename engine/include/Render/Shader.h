#pragma once

#include "Render/DXHelpers.h"

namespace Blainn
{
    struct BlainnShaderDesc
    {

    };

    class Shader
    {
    public:
        enum EShaderType : UINT
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

            SkyBoxVS,
            SkyBoxPS,

            UUIDVS,
            UUIDPS,

            NumShaders = 13U
        };

    public:
        explicit Shader() = default;
        ~Shader() noexcept = default;
    
        void AddDefines();
    
    private:
        BlainnShaderDesc m_shaderDescriptor;
    };
}