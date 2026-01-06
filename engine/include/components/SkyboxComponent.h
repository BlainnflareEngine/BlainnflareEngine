#pragma once

#include "EASTL/set.h"
#include "aliases.h"
#include "Handles/Handle.h"

namespace Blainn
{
struct SkyboxComponent
{
    SkyboxComponent() = default;

    SkyboxComponent(eastl::shared_ptr<TextureHandle>& handle)
    : textureHandle(handle)
    {
    }

    eastl::shared_ptr<TextureHandle> textureHandle;
};
} // namespace Blainn