#pragma once

#include "aliases.h"
#include "Handles/Handle.h"
#include "Render/FreyaCoreTypes.h"

namespace Blainn
{
    enum class ELightType
    {
        NONE = -1,
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

        LightComponent(const eastl::shared_ptr<MeshHandle> &meshHandle)
        {
            MeshHandle = meshHandle;
        }

        LightComponent(eastl::shared_ptr<MeshHandle>&& meshHandle)
        {
            MeshHandle = eastl::move(meshHandle);
        }

        eastl::shared_ptr<MeshHandle> MeshHandle;
        LightData Light;
        ELightType LightType = ELightType::NONE;
    };
} // namespace Blainn