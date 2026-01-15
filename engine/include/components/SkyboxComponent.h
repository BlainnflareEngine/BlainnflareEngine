#pragma once

#include "aliases.h"
#include "Handles/Handle.h"

namespace Blainn
{
struct SkyboxComponent
{
    SkyboxComponent()
    {
        textureHandle = eastl::make_shared<TextureHandle>(0);
    }

    SkyboxComponent(eastl::shared_ptr<TextureHandle>& handle)
    : textureHandle(handle)
    {
    }

    eastl::shared_ptr<TextureHandle> textureHandle;
};
} // namespace Blainn