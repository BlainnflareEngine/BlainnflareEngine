#pragma once

#include <EASTL/vector.h>

#include "aliases.h"
#include "runtime/scripting/LuaScript.h"

namespace Blainn
{
struct ScriptingComponent
{
    uuid parentId;
    uuid id;
    eastl::vector<LuaScript> scripts;
};
} // namespace Blainn
