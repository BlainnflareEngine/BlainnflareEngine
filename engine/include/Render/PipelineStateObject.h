#pragma once

#include "Render/DXHelpers.h"

namespace Blainn
{
    struct BlainnPipelineStateDesc
    {

    };

    class PipelineStateObject
    {
    public:
        enum EPsoType : UINT
        {
            CascadedShadowsOpaque = 0,

            DeferredGeometry,
            Wireframe,


            DeferredDirectional,
            
            DeferredPointWithinFrustum,
            DeferredPointIntersectsFarPlane,
            DeferredPointFullQuad,
            
            DeferredSpotWithinFrustum,
            DeferredSpotIntersectsFarPlane,
            DeferredSpotFullQuad,


            Transparency,
            Sky,
            Debug,

            UUID,

            NumPipelineStates
        };

    public:
        PipelineStateObject() = default;
        ~PipelineStateObject() noexcept = default;

    private:
        BlainnPipelineStateDesc m_psoDescriptor;
    };
}