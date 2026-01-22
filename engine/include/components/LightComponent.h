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

        LightComponent(LightComponent &&other)
        {

        }
        XMFLOAT4 Color = {1.0f, 1.0f, 1.0f, 1.0f};
    };

    struct DirectionalLight : public LightComponent
    {
        DirectionalLight()
        {

        }

        XMFLOAT3 Direction = {0.5f, -1.0f, 0.5f};
    };

    struct PointLight : public LightComponent
    {
        PointLight()
        {
        }

        float Range;
    };

    struct SpotLight : public LightComponent
    {
        SpotLight()
        {
        }

        XMFLOAT3 Direction = {0.5f, -1.0f, 0.5f};
        float Range;
        float SpotInnerAngle;
        float SpotOuterAngle;
    };

} // namespace Blainn