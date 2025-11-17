#pragma once

#include "handles/Handle.h"

namespace Blainn
{
struct MeshComponent
{
    MeshComponent(MeshHandle meshHandle)
        : m_meshHandle(meshHandle)
    {
    }
    MeshHandle m_meshHandle;
};
} // namespace Blainn