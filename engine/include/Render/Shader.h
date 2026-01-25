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

            DeferredPointVS,
            DeferredPointPS,
            
            DeferredSpotVS,
            DeferredSpotPS,

            SkyBoxVS,
            SkyBoxPS,

            UUIDVS,
            UUIDPS,

            NumShaders
        };

    public:
        explicit Shader() = default;
        ~Shader() noexcept = default;
    
        void AddDefines();
    
    private:
        BlainnShaderDesc m_shaderDescriptor;
    };
}