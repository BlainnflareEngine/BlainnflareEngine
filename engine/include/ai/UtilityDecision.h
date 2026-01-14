#pragma once

#include <sol/sol.hpp>
#include "EASTL/string.h"

namespace Blainn
{
struct UtilityDecision
{
    eastl::string name;
    eastl::string BTName;

    sol::function scoreFn;

    float cooldown = 0.0f;
};

struct UtilityDecisionState
{
    float cooldownRemaining = 0.0f;
};
}