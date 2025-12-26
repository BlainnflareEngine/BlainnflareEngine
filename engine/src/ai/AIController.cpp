#include <pch.h>
#include "ai/AIController.h"


namespace Blainn
{
void AIController::Init(
    BTMap trees,
    std::unique_ptr<UtilitySelector> utility,
    std::unique_ptr<Blackboard> blackboard
)
{
    m_trees = std::move(trees);
    m_utility = std::move(utility);
    m_blackboard = std::move(blackboard);

    m_activeTree = nullptr;
    m_activeTreeName.clear();
    m_activeDecisionName.clear();
}

void AIController::Update(float dt)
{
    if (!m_utility)
        return;

    m_utilityContext.UpdateCooldowns(dt);

    std::string decision =
        m_utility.get()->Evaluate(m_utilityContext, *m_blackboard, dt);

    if (decision.empty())
        return;

    m_activeDecisionName = decision;
    std::string btName = m_utility.get()->FindDecisionBTName(m_activeDecisionName);

    if ( btName.empty() && btName != m_activeTreeName )
    {
        SetActiveBT(btName);
    }

    if (!m_activeTree)
        return;

    BTStatus status = m_activeTree->Update(*m_blackboard);

    // TODO: status == Success or Failure start tree again, if aborted ???
    if (status == BTStatus::Success ||
        status == BTStatus::Failure ||
        status == BTStatus::Aborted)
    {
        m_activeTree->Reset();
        m_activeTree = nullptr;
        m_activeTreeName.clear();
        m_activeDecisionName.clear();
    }
}

void AIController::SetActiveBT(const std::string& treeName)
{
    auto it = m_trees.find(treeName);
    if (it == m_trees.end())
    {
        BF_ERROR("AIController: BT not found: " + treeName);
        return;
    }

    if (m_activeTree)
        m_activeTree->Reset(); // TODO: Make sure that reset is working after implementation will start existing

    m_activeTree = it->second.get();
    m_activeTreeName = treeName;
}
} // namespace Blainn