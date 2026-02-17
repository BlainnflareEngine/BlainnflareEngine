#pragma once

#include "aliases.h"

#include "scripting/LuaScript.h"
#include <EASTL/string.h>
#include <EASTL/unordered_map.h>

namespace Blainn
{

struct ScriptInfo
{
    bool shouldTriggerStart = true;
};

struct ScriptingComponent
{
    eastl::unordered_map<eastl::string, ScriptInfo> scriptPaths;
    eastl::unordered_map<uuid, LuaScript> scripts;
};
} // namespace Blainn
