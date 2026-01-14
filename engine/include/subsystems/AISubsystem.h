#pragma once

#include "helpers.h"
#include "aliases.h"
#include "ai/BehaviourTree.h"
#include "ai/UtilityBuilder.h"
#include "ai/BTBuilder.h"
#include "ai/AIController.h"
#include "components/AIControllerComponent.h"
#include "scene/Entity.h"

namespace Blainn
{
class AISubsystem
{
public:
    NO_COPY_NO_MOVE(AISubsystem);

    static AISubsystem &GetInstance();

    void Init();
    void Destroy();

    void Update(float dt);

    void CreateAttachAIControllerComponent(Entity entity, const Path &aiScriptPath);
    bool CreateAIController(Entity entity);
    void DestroyAIControllerComponent(Entity entity);

    BehaviourTree *GetBehaviourTree(const eastl::string &name);

private:
    AISubsystem() = default;

    void LoadBlackboard(const sol::table &scriptEnvironment, eastl::unique_ptr<Blackboard> &blackboard);
    void LoadBehaviourTrees(const sol::table &scriptEnvironment, BTMap &behaviourTrees);
    void LoadUtility(const sol::table &scriptEnvironment, eastl::unique_ptr<UtilitySelector> &utility);
};
} // namespace Blainn