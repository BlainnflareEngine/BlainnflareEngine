#pragma once
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

    UtilitySelector( std::vector<UtilityDecision> decisions, Settings settings = {} );

    std::string Evaluate( UtilityContext& context, Blackboard& blackboard, float deltaTime );
    std::string FindDecisionBTName( std::string decisionName );

private:
    std::vector<UtilityDecision> m_decisions;
    Settings m_settings;
};
}