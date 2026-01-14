#pragma once

#include <string>
#include <unordered_map>
#include <variant>
#include "ai/AIController.h"

namespace Blainn
{
class LuaScript;

using BlackboardValue = std::variant<int, float, bool, std::string>;

struct AIControllerComponent
{
    std::string scriptPath;
    AIController aiController;
    eastl::unique_ptr<LuaScript> aiScript;

    float MovementSpeed = 2;
    float StoppingDistance = 0.5;
};

} // namespace Blainn