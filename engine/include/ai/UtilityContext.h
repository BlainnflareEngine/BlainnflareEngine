#pragma once

#include "EASTL/unordered_map.h"
#include "ai/UtilityDecision.h"

namespace Blainn
{
class UtilityContext
{
public:
    eastl::unordered_map<eastl::string, UtilityDecisionState> states;
    eastl::string currentDecision;

    void UpdateCooldowns(float dt);
};
}