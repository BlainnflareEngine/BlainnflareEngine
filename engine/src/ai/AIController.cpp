#include <pch.h>
#include "ai/AIController.h"

#include "Navigation/NavigationSubsystem.h"


namespace Blainn
{
void AIController::Init(BTMap trees, std::unique_ptr<UtilitySelector> utility, std::unique_ptr<Blackboard> blackboard)
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
    if (!m_utility) return;

    m_utilityContext.UpdateCooldowns(dt);

    std::string newDecision = m_utility.get()->Evaluate(m_utilityContext, *m_blackboard, dt);

    if (!m_activeTree)
    {
        if (!newDecision.empty())
        {
            ActivateDecision(newDecision);
        }
        return;
    }

    std::string btName;
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
        CleanupActiveTree(); // TODO: Надо ли после этого запускать некст decision?
        /*
        if ( !newDecision.empty() ) // если дерево было и закончилось и есть новое решение, то запускаем следующее
        дерево
        {
            ActivateDecision(newDecision);
        }
        */
        return;
    case BTStatus::Error:
        HandleBTError();
        return;
    default:
        break;
    }

    // if ( status == BTStatus::Running )
    //     return;

    // CleanupActiveTree(); // если завершился любым способом
}


void AIController::Possess(const Entity &entity)
{
    m_controlledEntity = entity;
}


bool AIController::MoveTo(const Vec3 &target)
{
    if (!m_controlledEntity.IsValid())
    {
        BF_WARN("AIController: Controller is not possessing any entity.")
        return false;
    }

    if (!NavigationSubsystem::IsNavMeshLoaded())
    {
        BF_WARN("AIController: No loaded navigation mesh.")
        return false;
    }

    Vec3 start = m_controlledEntity.GetComponent<TransformComponent>().GetTranslation();

    eastl::vector<Vec3> path;
    path.reserve(16);

    if (!NavigationSubsystem::FindPath(start, target, path))
    {
        BF_WARN("AIController::MoveTo: Path not found from ({}, {}, {}) to ({}, {}, {}).", start.x, start.y, start.z,
                target.x, target.y, target.z);

        StopMoving();
        return false;
    }

    m_moveToTarget = target;
    m_currentPath = eastl::move(path);
    m_pathIndex = 0;
    StartMoving();

    return true;
}


void AIController::StopMoving()
{
    m_isMoving = false;
}


void AIController::StartMoving()
{
    m_isMoving = true;
}


bool AIController::GetDesiredDirection(Vec3 &outDirection, float stoppingDistance)
{
    if (!m_isMoving || m_currentPath.empty() || m_pathIndex >= m_currentPath.size())
    {
        m_isMoving = false;
        return false;
    }

    if (!m_controlledEntity.HasComponent<TransformComponent>()) return false;

    Vec3 agentPos = m_controlledEntity.GetComponent<TransformComponent>().GetTranslation();
    Vec3 targetPoint = m_currentPath[m_pathIndex];
    float distance = (targetPoint - agentPos).Length();

    if (distance <= stoppingDistance)
    {
        m_pathIndex++;
        if (m_pathIndex >= m_currentPath.size())
        {
            m_isMoving = false;
            return false;
        }
        targetPoint = m_currentPath[m_pathIndex];
    }

    m_moveDirection = targetPoint - agentPos;
    m_moveDirection.Normalize();
    outDirection = m_moveDirection;
    return true;
}


void AIController::ActivateDecision(const std::string &decision)
{
    m_activeDecisionName = decision;
    std::string btName = m_utility->FindDecisionBTName(decision);
    SetActiveBT(btName);
}

void AIController::CleanupActiveTree()
{
    if (m_activeTree) m_activeTree->Reset();

    m_activeTree = nullptr;
    m_activeTreeName.clear();
    m_activeDecisionName.clear();
    m_abortRequested = false;
}

void AIController::SetActiveBT(const std::string &treeName)
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

    if (m_activeTree) m_activeTree->Reset();

    m_activeTree = nullptr;
    m_activeTreeName.clear();
    m_activeDecisionName.clear();
    m_abortRequested = false;
}

} // namespace Blainn