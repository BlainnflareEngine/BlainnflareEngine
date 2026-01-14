#pragma once

#include "EASTL/string.h"
#include "EASTL/unordered_map.h"
#include "EASTL/variant.h"
#include "ai/AIController.h"

namespace Blainn
{
class LuaScript;

using BlackboardValue = eastl::variant<int, float, bool, eastl::string>;

struct AIControllerComponent
{
    std::string scriptPath;
    AIController aiController;
    eastl::unique_ptr<LuaScript> aiScript;
};

} // namespace Blainn