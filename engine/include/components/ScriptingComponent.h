#pragma once

#include "aliases.h"

#include "scripting/LuaScript.h"

namespace Blainn
{

struct ScriptingComponent
{
    eastl::unordered_map<uuid, LuaScript> scripts;
};
} // namespace Blainn
