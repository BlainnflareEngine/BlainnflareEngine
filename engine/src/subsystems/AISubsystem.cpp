#include <pch.h>
#include "subsystems/AISubsystem.h"
#include "subsystems/ScriptingSubsystem.h"
#include "subsystems/PerceptionSubsystem.h"

#include "Engine.h"
#include "scene/Scene.h"
#include "scene/TransformComponent.h"
#include "components/CameraComponent.h"

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

void AISubsystem::Init(Settings &settings)
{
    settings.enableLOD = true;
    settings.lodNearDistance = 200.0f;
    settings.lodMidDistance = 500.0f;
    settings.lodFarDistance = 1000.0f;
    settings.lodNearUpdateInterval = 0.0f;
    settings.lodMidUpdateInterval = 0.1f;
    settings.lodFarUpdateInterval = 0.5f;

    SetSettings(settings);
    BF_INFO("AISubsystem Init");
}

void AISubsystem::Destroy()
{
    BF_INFO("AISubsystem Destroy");
}

void AISubsystem::Update(float dt)
{
    Scene &scene = *Engine::GetActiveScene();
    
    if (m_settings.enableLOD)
    {
        UpdateLOD();
    }
    
    const auto& view = scene.GetAllEntitiesWith<IDComponent, AIControllerComponent>();
    for (const auto &[entityHandle, idComp, aiControllerComponent] : view.each())
    {
        aiControllerComponent.aiController.Update(dt);
    }
}

void AISubsystem::UpdateLOD()
{
    Scene& scene = *Engine::GetActiveScene();
    
    Vec3 cameraPos{0.0f};
    auto cameras = scene.GetAllEntitiesWith<IDComponent, TransformComponent, CameraComponent>();
    
    for (const auto& [entityHandle, idComp, transform, camera] : cameras.each())
    {
        if (camera.CameraPriority == 0)
        {
            Entity cameraEntity = scene.GetEntityWithUUID(idComp.ID);
            cameraPos = scene.GetWorldSpaceTransform(cameraEntity).GetTranslation();
            break;
        }
    }
    
    auto view = scene.GetAllEntitiesWith<IDComponent, TransformComponent, AIControllerComponent>();
    
    for (const auto& [entityHandle, idComp, transform, aiComp] : view.each())
    {
        Entity entity = scene.GetEntityWithUUID(idComp.ID);
        Vec3 entityPos = scene.GetWorldSpaceTransform(entity).GetTranslation();
        
        float distance = (entityPos - cameraPos).Length();
        
        float updateInterval = CalculateUpdateInterval(distance);
        aiComp.aiController.SetUpdateInterval(updateInterval);
    }
}

float AISubsystem::CalculateUpdateInterval(float distanceToCamera)
{
    if (distanceToCamera < m_settings.lodNearDistance)
        return m_settings.lodNearUpdateInterval;
    else if (distanceToCamera < m_settings.lodMidDistance)
        return m_settings.lodMidUpdateInterval;
    else if (distanceToCamera < m_settings.lodFarDistance)
        return m_settings.lodFarUpdateInterval;
    else
        return 1.0f; // Если далеко то раз в секунду
}

void AISubsystem::LoadBlackboard(const sol::table &scriptEnvironment, eastl::unique_ptr<Blackboard> &blackboard)
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
        blackboard->Set(key.c_str(), value);
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

        const eastl::string &name = tree->GetName();

        behaviourTrees.emplace(name, eastl::move(tree));
    }
}

void AISubsystem::LoadUtility(const sol::table &scriptEnvironment, eastl::unique_ptr<UtilitySelector> &utility)
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
        BF_ERROR("AI controller error: entity " + entity.GetUUID().str() + " already has AI controller component");
        return;
    }
    AIControllerComponent &component = entity.AddComponent<AIControllerComponent>();
    component.scriptPath = aiScriptPath.string();
}

bool AISubsystem::CreateAIController(Entity entity)
{
    AIControllerComponent *componentPtr = entity.TryGetComponent<AIControllerComponent>();
    if (!componentPtr)
    {
        BF_ERROR("AI controller error: entity " + entity.GetUUID().str() + " does not have AI controller component");
        return false;
    }

    componentPtr->aiController.Possess(entity);

    componentPtr->aiScript = ScriptingSubsystem::LoadAiScript(entity, componentPtr->scriptPath);
    if (!componentPtr->aiScript)
    {
        BF_ERROR("AI controller error: failed to load AI script for entity " + entity.GetUUID().str());
        entity.RemoveComponent<AIControllerComponent>();
        return false;
    }
    const sol::table &scriptEnv = componentPtr->aiScript->GetEnvironment();
    
    PerceptionComponent *perception = entity.TryGetComponent<PerceptionComponent>();
    if (perception)
    {
        sol::optional<sol::function> configurePerception = scriptEnv["ConfigurePerception"];
        if (configurePerception && perception)
        {
            auto result = configurePerception.value()(perception);
            if (!result.valid())
            {
                sol::error err = result;
                BF_ERROR("ConfigurePerception failed: " + eastl::string(err.what()));
            }
        }
    }

    StimulusComponent *stimulus = entity.TryGetComponent<StimulusComponent>();
    if (stimulus)
    {
        sol::optional<sol::function> configureStimulus = scriptEnv["ConfigureStimulus"];
        if (configureStimulus && stimulus)
        {
            auto result = configureStimulus.value()(stimulus);
            if (!result.valid())
            {
                sol::error err = result;
                BF_ERROR("ConfigureStimulus failed: " + eastl::string(err.what()));
            }
        }
    }

    eastl::unique_ptr<Blackboard> bb = eastl::make_unique<Blackboard>();
    LoadBlackboard(scriptEnv, bb);
    
    if (perception)
    {
        bb->Set("_perception", perception);
    }
    
    bb->Set("selfEntity", entity.GetUUID());

    BTMap trees;
    LoadBehaviourTrees(scriptEnv, trees);

    eastl::unique_ptr<UtilitySelector> utility;
    LoadUtility(scriptEnv, utility);

    componentPtr->aiController.Init(eastl::move(trees), eastl::move(utility), eastl::move(bb));
    
    BF_INFO("AI Controller created for entity: " + entity.GetUUID().str());
    
    return true;
}

void AISubsystem::DestroyAIControllerComponent(Entity entity)
{
    AIControllerComponent *componentPtr = entity.TryGetComponent<AIControllerComponent>();
    if (!componentPtr)
    {
        return;
    }
    componentPtr->aiController.HardReset();
    entity.RemoveComponent<AIControllerComponent>();
}