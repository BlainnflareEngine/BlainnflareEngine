#pragma once

#include "handles/Handle.h"

namespace Blainn
{
struct MeshComponent
{
    MeshComponent(const eastl::shared_ptr<MeshHandle> &meshHandle)
        : m_meshHandle(meshHandle)
    {
    }

    MeshComponent(eastl::shared_ptr<MeshHandle> &&meshHandle)
        : m_meshHandle(eastl::move(meshHandle))
    {
    }

    eastl::shared_ptr<MeshHandle> m_meshHandle;
};
} // namespace Blainn