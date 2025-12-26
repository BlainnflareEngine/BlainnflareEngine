#include <pch.h>
#include "ai/AIController.h"

namespace Blainn
{
void AIController::Init(
    std::unordered_map<std::string, BehaviourTree*> trees,
    std::unique_ptr<UtilitySelector> utility
)
{
    m_trees = std::move(trees);
    m_utility = std::move(utility);

    m_activeTree = nullptr;
    m_activeTreeName.clear();
    m_activeDecisionName.clear();
}

void AIController::Update(float dt)
{
    if (!m_utility)
        return;

    std::string decision =
        m_utility->Evaluate(m_utilityContext, m_blackboard, dt);

    if (decision.empty())
        return;

    if (decision != m_activeTreeName) // TODO: не сравнивать название дерева с названием решения
        SwitchTree(decision);

    if (m_activeTree)
    {
        BTStatus status = m_activeTree->Update(m_blackboard);

        // status == Success or Failure start tree again, if aborted ???
        if (status == BTStatus::Success ||
            status == BTStatus::Failure ||
            status == BTStatus::Aborted)
        {
            m_activeTree->Reset();
            m_activeTree = nullptr;
            m_activeTreeName.clear();
        }
    }
}

void AIController::SwitchTree(const std::string& treeName)
{
    auto it = m_trees.find(treeName);
    if (it == m_trees.end())
    {
        BF_ERROR("AIController: BT not found: " + treeName);
        return;
    }

    if (m_activeTree)
        m_activeTree->Reset(); // TODO: Make sure that reset is working after implementation will start existing

    m_activeTree = it->second;
    m_activeTreeName = treeName;
}
} // namespace Blainn