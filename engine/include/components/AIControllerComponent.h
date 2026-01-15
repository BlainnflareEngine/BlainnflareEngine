#pragma once

#include <string>
#include <variant>
#include "ai/AIController.h"
#include "scripting/LuaScript.h"

namespace Blainn
{

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