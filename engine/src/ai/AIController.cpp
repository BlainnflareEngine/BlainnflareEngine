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

    std::string newDecision =
        m_utility.get()->Evaluate(m_utilityContext, *m_blackboard, dt);

    if ( !m_activeTree )
    {
        if (!newDecision.empty())
        {
            ActivateDecision(newDecision);
        }
        return;
    }

    std::string btName;
    if ( !m_abortRequested && !newDecision.empty() && newDecision != m_activeDecisionName )
    {
        m_abortRequested = true;
        m_activeTree->RequestAbort();
    }

    BTStatus status = m_activeTree->Update(*m_blackboard);

    if ( status == BTStatus::Running )
        return;
    
    CleanupActiveTree(); // если завершился любым способом

    if ( !newDecision.empty() ) // если дерево было и закончилось и есть новое решение, то запускаем следующее дерево
    {
        ActivateDecision(newDecision);
    }
}

void AIController::ActivateDecision(const std::string& decision)
{
    m_activeDecisionName = decision;
    std::string btName = m_utility->FindDecisionBTName(decision);
    SetActiveBT(btName);
}

void AIController::CleanupActiveTree()
{
    if (m_activeTree)
        m_activeTree->Reset(); // TODO: make reset realisation

    m_activeTree = nullptr;
    m_activeTreeName.clear();
    m_activeDecisionName.clear();
    m_abortRequested = false;
}

void AIController::SetActiveBT(const std::string& treeName)
{
    auto it = m_trees.find(treeName);
    if (it == m_trees.end())
    {
        BF_ERROR("AIController: BT not found: " + treeName);
        return;
    }

    m_activeTree = it->second.get();
    m_activeTreeName = treeName;
}
} // namespace Blainn