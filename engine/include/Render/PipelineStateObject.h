#pragma once

#include "Render/DXHelpers.h"

namespace Blainnflare
{
    struct BlainnPipelineStateDesc
    {

    };

    class PipelineStateObject
    {
    public:
        enum class EPsoType : UINT
        {
            CascadedShadowsOpaque = 0,

            DeferredGeometry,
            Wireframe,

            DeferredDirectional,
            DeferredPointWithinFrustum,
            DeferredPointIntersectsFarPlane,
            DeferredPointFullQuad,
            DeferredSpot,

            Transparency,

            NumPipelineStates = 9u
        };

    public:
        PipelineStateObject() = default;
        ~PipelineStateObject() noexcept = default;

    private:
        BlainnPipelineStateDesc m_psoDescriptor;
    };
}