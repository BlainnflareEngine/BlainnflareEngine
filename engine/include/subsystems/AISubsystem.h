#pragma once

#include <unordered_map>
#include <memory>
#include "helpers.h"
#include "aliases.h"
#include "ai/BehaviourTree.h"
#include "ai/UtilityBuilder.h"
#include "ai/BTBuilder.h"
#include "ai/AIController.h"
#include "components/AIControllerComponent.h"

namespace Blainn
{
class AISubsystem
{
public:
    NO_COPY_NO_MOVE(AISubsystem);

    static AISubsystem& GetInstance();

    void Init(sol::state* lua);
    void Destroy();

    void Update(float dt);

    void CreateController(uuid entityID, const AIControllerComponent& component);
    void DestroyController(uuid entityID);

    BehaviourTree* GetBehaviourTree(const std::string& name);

private:
    AISubsystem() = default;

    void LoadBlackboard(std::unique_ptr<Blackboard> &blackboard);
    void LoadBehaviourTrees(BTMap &behaviourTrees);
    void LoadUtility(std::unique_ptr<UtilitySelector> &utility);

private:
    sol::state* m_lua = nullptr;

    std::unordered_map<uuid, AIController> m_controllers;
};
} // namespace Blainn