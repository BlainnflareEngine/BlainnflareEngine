#pragma once

#include <string>
#include <unordered_map>
#include <variant>

namespace Blainn
{
class LuaScript;

using BlackboardValue = std::variant<int, float, bool, std::string>;

struct AIControllerComponent
{
    std::string scriptPath;
    AIController aiController;
    eastl::unique_ptr<LuaScript> aiScript;
};

} // namespace Blainn