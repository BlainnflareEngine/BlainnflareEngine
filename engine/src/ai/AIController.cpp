#include <pch.h>
#include "ai/AIController.h"

namespace Blainn
{

void AIController::Init(
    BTMap trees,
    eastl::unique_ptr<UtilitySelector> utility,
    eastl::unique_ptr<Blackboard> blackboard
)
{
    m_trees = eastl::move(trees);
    m_utility = eastl::move(utility);
    m_blackboard = eastl::move(blackboard);

    m_activeTree = nullptr;
    m_activeTreeName.clear();
    m_activeDecisionName.clear();
}

bool AIController::ShouldUpdate(float dt)
{
    if (m_updateInterval <= 0.0f)
        return true;
    
    m_timeSinceLastUpdate += dt;
    
    if (m_timeSinceLastUpdate >= m_updateInterval)
    {
        m_timeSinceLastUpdate = 0.0f;
        return true;
    }
    
    return false;
}

void AIController::Update(float dt)
{
    if (!m_utility)
        return;
    
    if (!ShouldUpdate(dt))
        return;

    m_utilityContext.UpdateCooldowns(dt);

    eastl::string newDecision =
        m_utility.get()->Evaluate(m_utilityContext, *m_blackboard, dt);

    if (!m_activeTree)
    {
        if (!newDecision.empty())
        {
            ActivateDecision(newDecision);
        }
        return;
    }

    if (!m_abortRequested && !newDecision.empty() && newDecision != m_activeDecisionName)
    {
        m_abortRequested = true;
        m_activeTree->RequestAbort();
    }

    BTStatus status = m_activeTree->Update(*m_blackboard);

    switch (status)
    {
    case BTStatus::Running:
        return;
    case BTStatus::Success:
    case BTStatus::Failure:
    case BTStatus::Aborted:
        CleanupActiveTree();

        if (!newDecision.empty())
        {
            ActivateDecision(newDecision);
        }
        return;
    case BTStatus::Error:
        HandleBTError();
        return;
    default:
        break;
    }
}

void AIController::ActivateDecision(const eastl::string& decision)
{
    m_activeDecisionName = decision;
    eastl::string btName = m_utility->FindDecisionBTName(decision);
    SetActiveBT(btName);
}

void AIController::CleanupActiveTree()
{
    if (m_activeTree)
        m_activeTree->Reset();

    m_activeTree = nullptr;
    m_activeTreeName.clear();
    m_activeDecisionName.clear();
    m_abortRequested = false;
}

void AIController::SetActiveBT(const eastl::string& treeName)
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

void AIController::HandleBTError()
{
    BF_ERROR("AIController: BehaviourTree " + m_activeTreeName + " failed with ERROR");

    if (m_activeTree)
        m_activeTree->Reset();

    m_activeTree = nullptr;
    m_activeTreeName.clear();
    m_activeDecisionName.clear();
    m_abortRequested = false;
}

} // namespace Blainn