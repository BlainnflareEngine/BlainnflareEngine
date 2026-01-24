#pragma once

#include "aliases.h"
#include "Handles/Handle.h"
#include "Render/FreyaCoreTypes.h"

namespace Blainn
{
    enum class ELightType
    {
        Directional,
        Point,
        Spot,
        NumLightTypes
    };

    struct LightComponent
    {
    private:
        inline static const int kNumFramesMarkDirty = 3;

        // dirty flag available between frames
        int NumFramesDirty = kNumFramesMarkDirty; // NumFrameResources

    public:
        LightComponent()
        {
        }

        LightComponent(const LightComponent& other)
        {

        }

        LightComponent(LightComponent &&other) noexcept
        {

        }

        // call from qt
        void MarkFramesDirty()
        {
            NumFramesDirty = kNumFramesMarkDirty;
        };

        bool IsFramesDirty() const
        {
            return NumFramesDirty > 0;
        }

        void FrameResetDirtyFlags()
        {
            NumFramesDirty > 0 ? --NumFramesDirty : NumFramesDirty;
        }

        XMFLOAT4 Color = {1.0f, 1.0f, 1.0f, 1.0f};
    };

    struct DirectionalLightComponent : public LightComponent
    {
        DirectionalLightComponent()
        {

        }
    };

    struct PointLightComponent : public LightComponent
    {
        PointLightComponent()
        {

        }

        float FalloffEnd; // Range
        float FalloffStart;
    };

    struct SpotLightComponent : public LightComponent
    {
        SpotLightComponent()
        {
        }

        //XMFLOAT3 Direction = {0.5f, -1.0f, 0.5f};
        float Range;
        float SpotInnerAngle;
        float SpotOuterAngle;
    };

} // namespace Blainn