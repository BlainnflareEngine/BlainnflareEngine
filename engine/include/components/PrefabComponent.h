//
// Created by gorev on 05.02.2026.
//

#pragma once
#include "aliases.h"
#include "EASTL/any.h"
#include "EASTL/string.h"
#include "EASTL/unordered_map.h"
#include "EASTL/vector.h"

namespace Blainn
{
struct PrefabOverride
{
    eastl::string Path;
    eastl::any value;
};

// Component-Overrides map, stores prefab values that are overridden by user
using PrefabOverrides = eastl::unordered_map<entt::id_type, eastl::vector<PrefabOverride>>;

struct PrefabComponent
{
    Path Path;
    PrefabOverrides Overrides;
};
} // namespace Blainn
