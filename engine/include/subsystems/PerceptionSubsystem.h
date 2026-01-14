#pragma once

#include <eventpp/eventqueue.h>

#include "EASTL/unordered_map.h"
#include "EASTL/vector.h"
#include "helpers.h"
#include "aliases.h"

#include "scene/Entity.h"
#include "components/PerceptionComponent.h"
#include "components/StimulusComponent.h"
#include "ai/PerceptionEvents.h"

namespace Blainn
{

class PerceptionSubsystem
{
public:
    NO_COPY_NO_MOVE(PerceptionSubsystem);
    
    static PerceptionSubsystem& GetInstance();
    
    void Init();
    void Destroy();
    void Update(float dt);
    
    struct Settings
    {
        float defaultSightRange = 1500.0f;
        float defaultSoundRange = 2000.0f;
        bool enableLOD = true;
        float lodNearDistance = 1000.0f; // Каждый кадр
        float lodMidDistance = 3000.0f; // 0.1 сек
        float lodFarDistance = 5000.0f; // 0.5 сек
        float lodNearUpdateInterval = 0.0f; // Каждый кадр
        float lodMidUpdateInterval = 0.1f; // 10 раз в сек
        float lodFarUpdateInterval = 0.5f; // 2 раза в сек
    };
    
    void SetSettings(const Settings& settings) { m_settings = settings; }
    const Settings& GetSettings() const { return m_settings; }
    
    void CreateAttachPerceptionComponent(Entity entity);
    void DestroyPerceptionComponent(Entity entity);
    
    void CreateAttachStimulusComponent(Entity entity);
    void DestroyStimulusComponent(Entity entity);
    
    void RegisterStimulus(
        uuid sourceEntity,
        StimulusType type,
        const Vec3& location,
        float radius,
        const eastl::string& tag = ""
    );
    
    using PerceptionEventHandle = 
        eventpp::internal_::CallbackListBase<void(const PerceptionEventPointer&), 
        PerceptionEventPolicy>::Handle;
    
    static PerceptionEventHandle AddEventListener(
        const PerceptionEventType eventType,
        eastl::function<void(const PerceptionEventPointer&)> listener
    );
    
    static void RemoveEventListener(
        const PerceptionEventType eventType,
        const PerceptionEventHandle& handle
    );
    
private:
    PerceptionSubsystem() = default;
    
    void ProcessSightStimuli(float dt);
    void ProcessSoundStimuli(float dt);
    void ProcessTouchStimuli();
    void UpdateStimuliAge(float dt);
    void UpdateLOD();
    
    bool CheckLineOfSight(const Vec3& from, const Vec3& to);
    bool IsInFieldOfView(
        const Vec3& observerPos,
        const Quat& observerRotation,
        const Vec3& targetPos,
        float fovAngle
    );
    
    bool ShouldUpdatePerception(PerceptionComponent& perception, float dt);
    float CalculateUpdateInterval(float distanceToCamera);
    Vec3 GetForwardVector(const Quat& rotation);
    
    struct TemporaryStimulus
    {
        uuid sourceEntity;
        StimulusType type;
        Vec3 location;
        float radius;
        eastl::string tag;
        float lifetime;
        float age = 0.0f;
    };
    
    struct LOSCache
    {
        bool hasLineOfSight;
        float timeSinceLastCheck;
    };
    
    void ProcessEvents();
    
private:
    Settings m_settings;
    
    eastl::vector<TemporaryStimulus> m_temporaryStimuli;
    eastl::unordered_map<uint64_t, LOSCache> m_losCache;
    
    inline static eventpp::EventQueue<PerceptionEventType, 
        void(const PerceptionEventPointer&), PerceptionEventPolicy> s_perceptionEventQueue;
};

} // namespace Blainn