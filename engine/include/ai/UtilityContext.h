#pragma once

#include "EASTL/unordered_map.h"
#include "ai/UtilityDecision.h"

namespace Blainn
{
class UtilityContext
{
public:
    eastl::unordered_map<eastl::string, UtilityDecisionState> states; // TODO : понять как создавать стейты и сам контекст
    eastl::string currentDecision;

    void UpdateCooldowns(float dt);
};
}