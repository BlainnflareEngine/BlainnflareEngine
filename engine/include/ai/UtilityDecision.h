#pragma once

#include <sol/sol.hpp>

namespace Blainn
{
struct UtilityDecision
{
    std::string name;
    std::string BTName;

    sol::function scoreFn;

    float cooldown = 0.0f;
};

struct UtilityDecisionState
{
    float cooldownRemaining = 0.0f;
};
}