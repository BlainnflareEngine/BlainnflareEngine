#pragma once

#include "aliases.h"

#include "ai/BehaviourTree.h"
#include "ai/UtilitySelector.h"
#include "scene/Entity.h"

namespace Blainn
{

class AIController
{
public:
    AIController() = default;

    void Init(
        BTMap trees,
        eastl::unique_ptr<UtilitySelector> utility,
        eastl::unique_ptr<Blackboard> blackboard
    );

    void Update(float dt);

    void Possess(const Entity &entity);

    Blackboard &GetBlackboard()
    {
        return *m_blackboard;
    }
    
    bool MoveTo(const Vec3 &target);
    void StopMoving();
    void StartMoving();
    bool GetDesiredDirection(Vec3 &outDirection, float stoppingDistance);

    void SetUpdateInterval(float interval) { m_updateInterval = interval; }
    float GetUpdateInterval() const { return m_updateInterval; }
    bool ShouldUpdate(float dt);

    void HardReset();
    void ClearState();

private:
    void ActivateDecision(const eastl::string& decisionName);
    void SetActiveBT(const eastl::string& treeName);
    void CleanupActiveTree();
    void HandleBTError();

private:
    eastl::unique_ptr<UtilitySelector> m_utility;
    UtilityContext m_utilityContext;

    BTMap m_trees;
    BehaviourTree* m_activeTree = nullptr;
    eastl::string m_activeTreeName;
    eastl::string m_activeDecisionName;

    eastl::unique_ptr<Blackboard> m_blackboard;

    bool m_abortRequested = false;

    Vec3 m_moveToTarget = Vec3(0, 0, 0);
    bool m_isMoving = false;
    int m_pathIndex = 0;
    Vec3 m_moveDirection = Vec3(0, 0, 0);
    eastl::vector<Vec3> m_currentPath;

    Entity m_controlledEntity;
    
    // LOD
    float m_updateInterval = 0.0f; // 0 каждый кадр
    float m_timeSinceLastUpdate = 0.0f;
};

} // namespace Blainn