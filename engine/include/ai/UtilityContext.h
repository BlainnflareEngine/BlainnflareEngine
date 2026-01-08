#pragma once

#include "ai/UtilityDecision.h"

namespace Blainn
{
class UtilityContext
{
public:
    std::unordered_map<std::string, UtilityDecisionState> states; // TODO : понять как создавать стейты и сам контекст
    std::string currentDecision;

    void UpdateCooldowns(float dt);
};
}