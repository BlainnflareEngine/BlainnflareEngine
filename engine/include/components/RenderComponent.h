#pragma once

#include "handles/Handle.h"

namespace Blainn
{
struct RenderComponent
{
    bool m_visible = true;
    bool m_meshCanBeRendered = false;
    MeshHandle m_meshHandle{0};
};
} // namespace Blainn