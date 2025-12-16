#include <pch.h>
#include "ai/AIController.h"

namespace Blainn
{
void Blainn::AIController::Init(BTMap trees, std::unique_ptr<UtilitySelector> utility)
{
    m_trees = std::move(trees);
    m_utility = std::move(utility);
}

void Blainn::AIController::Update(float dt) 
{
    std::string decision = m_utility->Evaluate(m_utilityContext, m_blackboard, dt);

    if (decision.empty())
        return;

    auto it = m_trees.find(decision);
    if (it == m_trees.end())
        return;

    BTNode* nextTree = it->second.get();

    if (m_activeTree != nextTree)
    {
        // Do some staff when ending tree

        m_activeTree = nextTree;
    }

    m_activeTree->Update(m_blackboard);
}
} // namespace Blainn