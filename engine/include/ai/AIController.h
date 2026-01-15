#pragma once

#include "ai/BehaviourTree.h"
#include "ai/UtilitySelector.h"

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

    Blackboard& GetBlackboard() { return *m_blackboard; }
    
    void SetUpdateInterval(float interval) { m_updateInterval = interval; }
    float GetUpdateInterval() const { return m_updateInterval; }
    bool ShouldUpdate(float dt);

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
    
    // LOD
    float m_updateInterval = 0.0f; // 0 каждый кадр
    float m_timeSinceLastUpdate = 0.0f;
};

} // namespace Blainn