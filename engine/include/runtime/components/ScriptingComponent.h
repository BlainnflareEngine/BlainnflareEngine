#pragma once

#include <EASTL/unordered_map.h>

#include "aliases.h"
#include "runtime/scripting/LuaScript.h"

namespace Blainn
{
struct ScriptingComponent
{
    ScriptingComponent(const uuid &parentUuidIn)
        : parentId(parentUuidIn) {};

    uuid parentId;
    eastl::unordered_map<uuid, LuaScript> scripts;
};
} // namespace Blainn
