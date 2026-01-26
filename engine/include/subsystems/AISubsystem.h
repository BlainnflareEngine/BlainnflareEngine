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

    struct Settings
    {
        bool enableLOD = true;
        float lodNearDistance = 1000.0f;    // Каждый кадр
        float lodMidDistance = 3000.0f;     // 10 раз в сек
        float lodFarDistance = 5000.0f;     // 2 раза в сек
        float lodNearUpdateInterval = 0.0f; // Каждый кадр
        float lodMidUpdateInterval = 0.1f;  // 10 раз в сек
        float lodFarUpdateInterval = 0.5f;  // 2 раза в сек
    };

    static AISubsystem &GetInstance();

    void Init();
    void Init(Settings &settings);
    void Destroy();

    void Update(float dt);

    void CreateAttachAIControllerComponent(Entity entity, const Path &aiScriptPath);
    bool CreateAIController(Entity entity);
    void DestroyAIControllerComponent(Entity entity);

    BehaviourTree *GetBehaviourTree(const eastl::string &name);


    
    void SetSettings(const Settings& settings) { m_settings = settings; }
    const Settings& GetSettings() const { return m_settings; }

private:
    AISubsystem() = default;

    void LoadBlackboard(const sol::table &scriptEnvironment, eastl::unique_ptr<Blackboard> &blackboard);
    void LoadBehaviourTrees(const sol::table &scriptEnvironment, BTMap &behaviourTrees);
    void LoadUtility(const sol::table &scriptEnvironment, eastl::unique_ptr<UtilitySelector> &utility);
    
    void UpdateLOD();
    float CalculateUpdateInterval(float distanceToCamera);

private:
    Settings m_settings;
};

} // namespace Blainn