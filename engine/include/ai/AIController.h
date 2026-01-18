#pragma once

#include "aliases.h"


#include <unordered_map>
#include <string>

#include "ai/BehaviourTree.h"
#include "ai/UtilitySelector.h"
#include "scene/Entity.h"

namespace Blainn
{

class AIController
{
public:
    AIController() = default;

    void Init(BTMap trees, std::unique_ptr<UtilitySelector> utility, std::unique_ptr<Blackboard> blackboard);

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

private:
    void ActivateDecision(const std::string &decisionName);
    void SetActiveBT(const std::string &treeName);
    void CleanupActiveTree();
    void HandleBTError();

private:
    std::unique_ptr<UtilitySelector> m_utility;
    UtilityContext m_utilityContext;

    BTMap m_trees;
    BehaviourTree *m_activeTree = nullptr;
    std::string m_activeTreeName;
    std::string m_activeDecisionName;

    std::unique_ptr<Blackboard> m_blackboard;

    bool m_abortRequested = false;

    Vec3 m_moveToTarget = Vec3(0, 0, 0);
    bool m_isMoving = false;
    int m_pathIndex = 0;
    Vec3 m_moveDirection = Vec3(0, 0, 0);
    eastl::vector<Vec3> m_currentPath;

    Entity m_controlledEntity;
};

} // namespace Blainn