#pragma once

#include "handles/Handle.h"

namespace Blainn
{
struct RenderComponent
{
    bool m_visible = true;
    bool m_meshCanBeRendered = false; // TODO: replace with std::optional<MeshHandle> two fields
    eastl::shared_ptr<Blainn::MeshHandle> m_meshHandle = nullptr;
};
} // namespace Blainn