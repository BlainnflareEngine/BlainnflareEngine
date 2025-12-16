#pragma once

#include <pch.h>
#include "ai/BT.h"
#include "ai/UtilitySelector.h"

namespace Blainn
{
class AIController
{
public:
    void Init(BTMap trees, std::unique_ptr<UtilitySelector> utility);
    void Update(float dt);

private:

    Blackboard m_blackboard;
    UtilityContext m_utilityContext;
    std::unique_ptr<UtilitySelector> m_utility;
    BTMap m_trees;

    BTNode* m_activeTree = nullptr;
};
}