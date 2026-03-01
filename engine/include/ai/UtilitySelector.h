#pragma once

#include "EASTL/vector.h"
#include "ai/UtilityContext.h"
#include "ai/Blackboard.h"

namespace Blainn
{
class UtilitySelector
{
public:
    struct Settings
    {
        bool normalize = false;
        float hysteresis = 0.0f;
    };

    UtilitySelector(eastl::vector<UtilityDecision> decisions);
    UtilitySelector(eastl::vector<UtilityDecision> decisions, Settings settings);

    eastl::string Evaluate( UtilityContext& context, Blackboard& blackboard, float deltaTime );
    eastl::string FindDecisionBTName( eastl::string decisionName );

private:
    eastl::vector<UtilityDecision> m_decisions;
    Settings m_settings;
};
}
