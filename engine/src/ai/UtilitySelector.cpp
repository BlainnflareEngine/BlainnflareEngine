#include "ai/UtilitySelector.h"

Blainn::UtilitySelector::UtilitySelector(eastl::vector<UtilityDecision> decisions, Settings settings) 
    : m_decisions(eastl::move(decisions)), m_settings(settings) {}


eastl::string Blainn::UtilitySelector::Evaluate(UtilityContext &context, Blackboard &blackboard, float deltaTime)
{
    context.UpdateCooldowns(deltaTime);

    float bestScore = -FLT_MAX;
    eastl::string bestDecision;

    eastl::unordered_map<eastl::string, float> scores;
    float scoreSum = 0.0f;

    for (const auto& decision : m_decisions)
    {
        auto& state = context.states[decision.name];

        if (state.cooldownRemaining > 0.0f)
            continue;

        float score = decision.scoreFn(blackboard);

        if (score <= 0.0f)
            continue;

        scores[decision.name] = score;
        scoreSum += score;
    }

    if (scores.empty())
        return context.currentDecision;

    if (m_settings.normalize && scoreSum > 0.0f)
    {
        for (auto& [_, score] : scores)
            score /= scoreSum;
    }

    for (const auto& [name, score] : scores)
    {
        if (score > bestScore)
        {
            bestScore = score;
            bestDecision = name;
        }
    }

    if (!context.currentDecision.empty() &&
        context.currentDecision != bestDecision &&
        m_settings.hysteresis > 0.0f)
    {
        float currentScore = scores[context.currentDecision];
        if (currentScore + m_settings.hysteresis >= bestScore)
        {
            return context.currentDecision;
        }
    }

    if (context.currentDecision != bestDecision)
    {
        for (const auto& decision : m_decisions)
        {
            if (decision.name == bestDecision && decision.cooldown > 0.0f)
            {
                context.states[decision.name].cooldownRemaining = decision.cooldown;
            }
        }
    }

    context.currentDecision = bestDecision;
    return bestDecision;
}

eastl::string Blainn::UtilitySelector::FindDecisionBTName(eastl::string decisionName)
{
    for (auto decision : m_decisions)
        if (decision.name == decisionName)
            return decision.BTName;
    return eastl::string();
}