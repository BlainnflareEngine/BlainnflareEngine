#pragma once

#include <unordered_map>
#include <string>

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
        std::unique_ptr<UtilitySelector> utility,
        std::unique_ptr<Blackboard> blackboard
    );

    void Update(float dt);

    Blackboard& GetBlackboard() { return *m_blackboard; }

private:
    void ActivateDecision(const std::string& decisionName);
    void SetActiveBT(const std::string& treeName);
    void CleanupActiveTree();

private:
    std::unique_ptr<UtilitySelector> m_utility;
    UtilityContext m_utilityContext;

    BTMap m_trees;
    BehaviourTree* m_activeTree = nullptr;
    std::string m_activeTreeName;
    std::string m_activeDecisionName;

    std::unique_ptr<Blackboard> m_blackboard;

    bool m_abortRequested = false;
};

} // namespace Blainn