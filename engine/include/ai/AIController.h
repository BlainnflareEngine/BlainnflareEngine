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
        std::unordered_map<std::string, BehaviourTree*> trees, // TODO: Currently controller doesn't own bt, mb it's wrong, mb utility doesn't need to be in here too, idk, nada podumot'
        std::unique_ptr<UtilitySelector> utility
    );

    void Update(float dt);

    Blackboard& GetBlackboard() { return m_blackboard; }

private:
    void SwitchTree(const std::string& treeName);

private:
    Blackboard m_blackboard;
    UtilityContext m_utilityContext;

    std::unique_ptr<UtilitySelector> m_utility;

    std::unordered_map<std::string, BehaviourTree*> m_trees;

    BehaviourTree* m_activeTree = nullptr;
    std::string m_activeTreeName;
    std::string m_activeDecisionName;
};

} // namespace Blainn