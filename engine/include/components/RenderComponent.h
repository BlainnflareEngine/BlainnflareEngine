#pragma once

#include "handles/Handle.h"

namespace Blainn
{
struct RenderComponent
{
    bool m_visible = true;
    bool m_meshCanBeRendered = false; // TODO: replace with std::optional<MeshHandle> two fields
    MeshHandle m_meshHandle{0};
};
} // namespace Blainn