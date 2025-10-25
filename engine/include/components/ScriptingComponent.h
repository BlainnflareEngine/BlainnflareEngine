#pragma once

#include <EASTL/unordered_map.h>

#include "aliases.h"

#include "scripting/LuaScript.h"

namespace Blainn
{

struct ScriptingComponent
{
    // uuid parentId;TODO: store Entity here?
    eastl::unordered_map<uuid, LuaScript> scripts;
};
} // namespace Blainn
