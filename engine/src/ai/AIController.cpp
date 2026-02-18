#include <pch.h>
#include "ai/AIController.h"

#include "Engine.h"
#include "Navigation/NavigationSubsystem.h"
#include "components/AIControllerComponent.h"


namespace Blainn
{

void AIController::Init(BTMap trees, eastl::unique_ptr<UtilitySelector> utility,
                        eastl::unique_ptr<Blackboard> blackboard)
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
    if (m_updateInterval <= 0.0f) return true;

    m_timeSinceLastUpdate += dt;

    if (m_timeSinceLastUpdate >= m_updateInterval)
    {
        m_timeSinceLastUpdate = 0.0f;
        return true;
    }

    return false;
}

void AIController::ClearState()
{
    m_activeTree->ClearState();
}

void AIController::HardReset()
{
    if (m_activeTree) m_activeTree->HardReset();

    m_activeTree = nullptr;
    m_activeTreeName.clear();
    m_activeDecisionName.clear();
    m_abortRequested = false;

    m_trees.clear();
    m_utility.release();
    m_blackboard.release();
}

void AIController::Update(float dt)
{
    if (!m_utility) return;

    if (!ShouldUpdate(dt)) return;

    m_utilityContext.UpdateCooldowns(dt);

    eastl::string newDecision = m_utility.get()->Evaluate(m_utilityContext, *m_blackboard, dt);

    if (!m_activeTree)
    {
        if (!newDecision.empty())
        {
            ActivateDecision(newDecision);
        }
        return;
    }

    eastl::string btName;
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
        if (!newDecision.empty() && newDecision != m_activeDecisionName)
        {
            CleanupActiveTree();
            ActivateDecision(newDecision);
        }
        else
        {
            ClearState();
        }
        return;
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
        BF_ERROR("AIController::Update: Unknown BTStatus");
        break;
    }
}

void AIController::Possess(const Entity &entity)
{
    m_controlledEntity = entity;
}


bool AIController::MoveTo(const Vec3 &target)
{
    if (!m_controlledEntity.IsValid())
    {
        BF_WARN("AIController: Controller is not possessing any entity.");
        return false;
    }

    if (!NavigationSubsystem::IsNavMeshLoaded())
    {
        BF_WARN("AIController: No loaded navigation mesh.");
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


bool AIController::IsMoving() const
{
    return m_isMoving;
}

bool AIController::GetDesiredDirection(Vec3 &outDirection)
{
    return GetDesiredDirection(outDirection, DesiredDirectionRequest{});
}


bool AIController::GetDesiredDirection(Vec3 &outDirection, const DesiredDirectionRequest &request)
{
    if (!m_isMoving || m_currentPath.empty() || m_pathIndex >= m_currentPath.size())
    {
        m_isMoving = false;
        return false;
    }

    if (!m_controlledEntity.HasComponent<TransformComponent>()) return false;

    Vec3 agentPos = m_controlledEntity.GetComponent<TransformComponent>().GetTranslation();
    Vec3 targetPoint = m_currentPath[m_pathIndex];

    Vec3 adjustedTarget = targetPoint;
    if (m_pathIndex != 0)
    {
        adjustedTarget.y += request.offset;
    }

    float distance = (adjustedTarget - agentPos).Length();

    if (distance <= request.stoppingDistance)
    {
        m_pathIndex++;
        if (m_pathIndex >= m_currentPath.size())
        {
            m_isMoving = false;
            return false;
        }

        targetPoint = m_currentPath[m_pathIndex];
        adjustedTarget = targetPoint;
        if (m_pathIndex < m_currentPath.size() - 1)
        {
            adjustedTarget.y += request.offset;
        }
    }

    m_moveDirection = adjustedTarget - agentPos;
    m_moveDirection.Normalize();
    outDirection = m_moveDirection;
    return true;
}


void AIController::RotateControlledPawn(const Vec3 &LookTo)
{
    Vec3 flatDir(LookTo.x, 0.0f, LookTo.z);
    if (flatDir.LengthSquared() > 1e-6f)
    {
        flatDir.Normalize();
        float angle = atan2f(flatDir.x, flatDir.z);
        Quat rot = Quat::CreateFromYawPitchRoll(angle, 0.0f, 0.0f);
        if (auto comp = m_controlledEntity.TryGetComponent<TransformComponent>()) comp->SetRotation(rot);
    }
}


void AIController::RotateControlledPawnLerp(const Vec3 &LookTo)
{
    TransformComponent *transform = m_controlledEntity.TryGetComponent<TransformComponent>();
    AIControllerComponent *aiControllerComp = m_controlledEntity.TryGetComponent<AIControllerComponent>();

    if (!transform || !aiControllerComp) return;

    Vec3 flatDir(LookTo.x, 0.0f, LookTo.z);
    if (flatDir.LengthSquared() > 1e-6f)
    {
        flatDir.Normalize();
        float angle = atan2f(flatDir.x, flatDir.z);
        Quat currentRot = transform->GetRotation();
        Quat targetRot = Quat::CreateFromYawPitchRoll(angle, 0.0f, 0.0f);
        Quat newRot =
            Quat::Slerp(currentRot, targetRot,
                        eastl::min(1.0f, aiControllerComp->RotationSpeed * (XM_PI / 180.0f) * Engine::GetDeltaTime()));

        if (auto comp = m_controlledEntity.TryGetComponent<TransformComponent>()) comp->SetRotation(newRot);
    }
}

void AIController::ActivateDecision(const eastl::string &decision)
{
    m_activeDecisionName = decision;
    eastl::string btName = m_utility->FindDecisionBTName(decision);
    SetActiveBT(btName);

    m_abortRequested = false;

    if (m_activeTree)
    {
        m_activeTree->ClearState();
    }
}

void AIController::CleanupActiveTree()
{
    if (m_activeTree) m_activeTree->HardReset();

    m_activeTree = nullptr;
    m_activeTreeName.clear();
    m_activeDecisionName.clear();
    m_abortRequested = false;
}

void AIController::SetActiveBT(const eastl::string &treeName)
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

    if (m_activeTree) m_activeTree->HardReset();

    m_activeTree = nullptr;
    m_activeTreeName.clear();
    m_activeDecisionName.clear();
    m_abortRequested = false;
}

} // namespace Blainn
