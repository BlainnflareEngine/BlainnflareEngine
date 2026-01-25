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
        float Intensity = 1.0f;
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

        float FalloffEnd = 1.0f; // Range
        float FalloffStart = 0.1f;
    };

    struct SpotLightComponent : public LightComponent
    {
        SpotLightComponent()
        {
        }

        float FalloffEnd = 1.0f; // Range
        float FalloffStart = 0.1f;
        float SpotInnerAngle;
        float SpotOuterAngle;
    };

} // namespace Blainn