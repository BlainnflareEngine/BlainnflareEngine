#pragma once

#include "aliases.h"

#include "scripting/LuaScript.h"

namespace Blainn
{

struct ScriptInfo
{
    bool shouldTriggerStart = true;
};

struct ScriptingComponent
{
    eastl::unordered_map<eastl::string, ScriptInfo> scriptPaths;
    eastl::unordered_map<uuid, eastl::shared_ptr<LuaScript>> scripts;
};
} // namespace Blainn
