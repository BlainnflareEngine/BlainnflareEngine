#include <pch.h>
#include "subsystems/AISubsystem.h"
#include "subsystems/ScriptingSubsystem.h"

#include "Engine.h"
#include "scene/Scene.h"

using namespace Blainn;

AISubsystem &AISubsystem::GetInstance()
{
    static AISubsystem instance;
    return instance;
}

void AISubsystem::Init()
{
    BF_INFO("AISubsystem Init");
}

void AISubsystem::Destroy()
{
    BF_INFO("AISubsystem Destroy");
}

void AISubsystem::Update(float dt)
{
    Scene &scene = *Engine::GetActiveScene();
    auto view = scene.GetAllEntitiesWith<AIControllerComponent>();
    for (const auto &[entity, aiControllerComponent] : view.each())
    {
        aiControllerComponent.aiController.Update(dt);
    }
}

void AISubsystem::LoadBlackboard(const sol::table &scriptEnvironment, std::unique_ptr<Blackboard> &blackboard)
{
    sol::table bbTable = scriptEnvironment["Blackboard"];
    if (!bbTable.valid())
    {
        BF_WARN("AISubsystem: no Blackboard in Lua");
        return;
    }

    for (auto &kv : bbTable)
    {
        std::string key = kv.first.as<std::string>();
        sol::object value = kv.second;
        blackboard->Set(key, value);
    }
}

void AISubsystem::LoadBehaviourTrees(const sol::table &scriptEnvironment, BTMap &behaviourTrees)
{
    sol::table btTable = scriptEnvironment["BehaviourTrees"];
    if (!btTable.valid())
    {
        BF_WARN("AISubsystem: no BehaviourTrees in Lua");
        return;
    }

    for (auto &kv : btTable)
    {
        sol::table bt = kv.second.as<sol::table>();

        BTBuilder builder;
        auto tree = builder.BuildFromLua(bt);

        if (!tree) continue;

        const std::string &name = tree->GetName();
        BF_INFO("Loaded BehaviourTree: " + name);

        behaviourTrees.emplace(name, std::move(tree));
    }
}

void AISubsystem::LoadUtility(const sol::table &scriptEnvironment, std::unique_ptr<UtilitySelector> &utility)
{
    sol::table utilityTable = scriptEnvironment["Utility"];

    if (!utilityTable.valid())
    {
        BF_WARN("AISubsystem: no Utility in Lua");
        return;
    }

    utility = UtilityBuilder::Build(utilityTable);
}

void AISubsystem::CreateAttachAIControllerComponent(Entity entity, const Path &aiScriptPath)
{
    AIControllerComponent *componentPtr = entity.TryGetComponent<AIControllerComponent>();
    if (componentPtr)
    {
        BF_ERROR("AI controller error: entity " + entity.GetUUID().str() + "already has AI controller component");
        return;
    }
    AIControllerComponent &component = entity.AddComponent<AIControllerComponent>();
    component.scriptPath = aiScriptPath.string();
}


bool Blainn::AISubsystem::CreateAIController(Entity entity)
{
    AIControllerComponent *componentPtr = entity.TryGetComponent<AIControllerComponent>();
    if (!componentPtr)
    {
        BF_ERROR("AI controller error: entity " + entity.GetUUID().str() + "does not have AI controller component");
        return false;
    }

    componentPtr->aiScript = ScriptingSubsystem::LoadAiScript(entity, componentPtr->scriptPath);
    if (!componentPtr->aiScript)
    {
        BF_ERROR("AI controller error: failed to load AI script for entity " + entity.GetUUID().str());
        entity.RemoveComponent<AIControllerComponent>();
        return false;
    }
    const sol::table &scriptEnv = componentPtr->aiScript->GetEnvironment();

    std::unique_ptr<Blackboard> bb;
    LoadBlackboard(scriptEnv, bb);

    BTMap trees;
    LoadBehaviourTrees(scriptEnv, trees);

    std::unique_ptr<UtilitySelector> utility;
    LoadUtility(scriptEnv, utility);

    componentPtr->aiController.Init(std::move(trees), std::move(utility), std::move(bb));
    return true;
}

void AISubsystem::DestroyAIControllerComponent(Entity entity)
{
    AIControllerComponent *componentPtr = entity.TryGetComponent<AIControllerComponent>();
    if (!componentPtr)
    {
        BF_ERROR("AI controller destroy error: entity " + entity.GetUUID().str()
                 + "does not have AI controller component");
        return;
    }
    entity.RemoveComponent<AIControllerComponent>();
}