#include "ai/UtilityContext.h"

void Blainn::UtilityContext::UpdateCooldowns(float dt)
{
    for (auto& [_, state] : states)
    {
        state.cooldownRemaining = eastl::max(0.0f, state.cooldownRemaining - dt);
    }
}