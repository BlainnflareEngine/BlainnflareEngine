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
        LightComponent()
        {
        }

        LightComponent(const LightComponent& other)
        {

        }

        LightComponent(LightComponent &&other) noexcept
        {

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

        float Range;
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