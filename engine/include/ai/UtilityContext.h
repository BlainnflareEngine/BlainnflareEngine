#pragma once

#include "ai/UtilityDecision.h"

namespace Blainn
{
class UtilityContext
{
public:
    std::unordered_map<std::string, UtilityDecisionState> states;
    std::string currentDecision;

    void UpdateCooldowns(float dt);
};
}