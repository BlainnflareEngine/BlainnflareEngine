#include <pch.h>
#include "subsystems/AISubsystem.h"
#include "AISubsystem.h"

using namespace Blainn;

AISubsystem& AISubsystem::GetInstance()
{
    static AISubsystem instance;
    return instance;
}

void AISubsystem::Init(sol::state* lua)
{
    BF_INFO("AISubsystem Init");

    m_lua = lua;
}

void AISubsystem::Destroy()
{
    BF_INFO("AISubsystem Destroy");

    m_controllers.clear();
    m_lua = nullptr;
}

void AISubsystem::Update(float dt)
{
    for (auto& [entity, controller] : m_controllers)
    {
        controller.Update(dt);
    }
}

void AISubsystem::LoadBlackboard(std::unique_ptr<Blackboard> &blackboard)
{
    sol::table bbTable = (*m_lua)["Blackboard"];
    if (!bbTable.valid())
    {
        BF_WARN("AISubsystem: no Blackboard in Lua");
        return;
    }
    
    for (auto& kv : bbTable)
    {
        std::string key = kv.first.as<std::string>();
        sol::object value = kv.second;
        blackboard->Set(key, value);
    }
}

void AISubsystem::LoadBehaviourTrees(std::unordered_map<std::string, std::unique_ptr<BehaviourTree>> &behaviourTrees)
{
    if (!m_lua)
        return;

    sol::table btTable = (*m_lua)["BehaviourTrees"];
    if (!btTable.valid())
    {
        BF_WARN("AISubsystem: no BehaviourTrees in Lua");
        return;
    }

    for (auto& kv : btTable)
    {
        sol::table bt = kv.second.as<sol::table>();

        BTBuilder builder;
        auto tree = builder.BuildFromLua(bt);

        if (!tree)
            continue;

        const std::string& name = tree->GetName();
        BF_INFO("Loaded BehaviourTree: " + name);

        behaviourTrees.emplace(name, std::move(tree));
    }
}

void AISubsystem::LoadUtility(std::unique_ptr<UtilitySelector> &utility)
{
    sol::table utilityTable = (*m_lua)["Utility"];

    if (!utilityTable.valid())
    {
        BF_WARN("AISubsystem: no Utility in Lua");
        return;
    }

    utility = UtilityBuilder::Build(utilityTable);
}

void AISubsystem::CreateController(uuid entityID, const AIControllerComponent& component)
{
    if (m_controllers.contains(entityID))
        return;

    std::unique_ptr<Blackboard> bb;
    LoadBlackboard(bb);

    BTMap trees;
    LoadBehaviourTrees(trees);

    std::unique_ptr<UtilitySelector> utility;
    LoadUtility(utility);

    AIController controller;
    controller.Init(
        std::move(trees),
        std::move(utility),
        std::move(bb)
    );

    m_controllers.emplace(entityID, std::move(controller));
}

void AISubsystem::DestroyController(uuid entityID)
{
    m_controllers.erase(entityID);
}