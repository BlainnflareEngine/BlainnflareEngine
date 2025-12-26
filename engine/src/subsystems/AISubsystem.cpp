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

    LoadBehaviourTrees();
}

void AISubsystem::Destroy()
{
    BF_INFO("AISubsystem Destroy");

    m_controllers.clear();
    m_behaviourTrees.clear();
    m_lua = nullptr;
}

void AISubsystem::Update(float dt)
{
    for (auto& [entity, controller] : m_controllers)
    {
        controller.Update(dt);
    }
}

void AISubsystem::LoadBehaviourTrees()
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

        m_behaviourTrees.emplace(name, std::move(tree));
    }
}

void AISubsystem::CreateController(uuid entityID, const AIControllerComponent& component)
{
    if (m_controllers.contains(entityID))
    {
        BF_WARN("AIController already exists for entity");
        return;
    }

    std::unordered_map<std::string, BehaviourTree*> trees;
    for (const auto& name : component.behaviourTrees)
    {
        auto it = m_behaviourTrees.find(name);
        if (it == m_behaviourTrees.end())
        {
            BF_ERROR("Unknown BehaviourTree: " + name);
            continue;
        }
        trees[name] = it->second.get();
    }

    sol::table utilities = (*m_lua)["Utilities"];
    sol::table utilityTable = utilities[component.utilityProfile];

    if (!utilityTable.valid())
    {
        BF_ERROR("Utility profile not found: " + component.utilityProfile);
        return;
    }

    auto utility = UtilityBuilder::Build(utilityTable);

    AIController controller;
    controller.Init(std::move(trees), std::move(utility));

    for (auto& [key, value] : component.initialBlackboard) // init BB
    {
        std::visit([&](auto&& v)
        {
            controller.GetBlackboard().Set(key, v);
        }, value);
    }

    m_controllers.emplace(entityID, std::move(controller));
}

void AISubsystem::DestroyController(uuid entityID)
{
    m_controllers.erase(entityID);
}

BehaviourTree* AISubsystem::GetBehaviourTree(const std::string& name)
{
    auto it = m_behaviourTrees.find(name);
    if (it == m_behaviourTrees.end())
        return nullptr;
    return it->second.get();
}