#pragma once

#include <eventpp/eventqueue.h>

#include <EASTL/unordered_map.h>
#include <EASTL/vector.h>
#include <EASTL/string.h>
#include <EASTL/shared_ptr.h>
#include <EASTL/functional.h>

#include "helpers.h"
#include "aliases.h"

#include "components/PerceptionComponent.h"
#include "components/StimulusComponent.h"

// Forward declarations
namespace Blainn
{
class Entity;
enum class StimulusType : uint8_t;
class PhysicsEvent;

enum class PerceptionEventType;
struct PerceptionEvent;
using PerceptionEventPointer = eastl::shared_ptr<PerceptionEvent>;
struct PerceptionEventPolicy;
}

namespace Blainn
{

class PerceptionSubsystem
{
public:
    NO_COPY_NO_MOVE(PerceptionSubsystem);

    struct Settings
    {
        float defaultSightRange = 1500.0f;
        float defaultSoundRange = 2000.0f;
        bool enableLOD = true;
        float lodNearDistance = 1000.0f;
        float lodMidDistance = 3000.0f;
        float lodFarDistance = 5000.0f;
        float lodNearUpdateInterval = 0.0f;
        float lodMidUpdateInterval = 0.1f;
        float lodFarUpdateInterval = 0.5f;
    };

    static PerceptionSubsystem &GetInstance();

    void Init();
    void Init(Settings &settings);
    void Destroy();
    void Update(float dt);



    void SetSettings(const Settings &settings)
    {
        m_settings = settings;
    }
    const Settings &GetSettings() const
    {
        return m_settings;
    }

    void CreateAttachPerceptionComponent(Entity entity);
    void DestroyPerceptionComponent(Entity entity);

    void CreateAttachStimulusComponent(Entity entity);
    void DestroyStimulusComponent(Entity entity);

    void RegisterStimulus(uuid sourceEntity, StimulusType type, const Vec3 &location, float radius,
                          const eastl::string &tag = "");

    using PerceptionEventHandle =
        eventpp::internal_::CallbackListBase<void(const PerceptionEventPointer &), PerceptionEventPolicy>::Handle;

    static PerceptionEventHandle AddEventListener(const PerceptionEventType eventType,
                                                  eastl::function<void(const PerceptionEventPointer &)> listener);

    static void RemoveEventListener(const PerceptionEventType eventType, const PerceptionEventHandle &handle);

    static void TouchListener(const eastl::shared_ptr<PhysicsEvent> &event);

private:
    PerceptionSubsystem() = default;

    void ProcessSightStimuli(float dt);
    void ProcessSoundStimuli(float dt);
    void ProcessTouchStimuli();
    void UpdateStimuliAge(float dt);
    void UpdateLOD();

    bool CheckLineOfSight(Entity &ignoreEntity, uuid desiredEntityID, const Vec3 &from, const Vec3 &to);
    bool IsInFieldOfView(const Vec3 &observerPos, const Quat &observerRotation, const Vec3 &targetPos, float fovAngle);

    bool ShouldUpdatePerception(PerceptionComponent &perception, float dt);
    float CalculateUpdateInterval(float distanceToCamera);
    Vec3 GetForwardVector(const Quat &rotation);

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

    inline static eventpp::EventQueue<PerceptionEventType, void(const PerceptionEventPointer &), PerceptionEventPolicy>
        s_perceptionEventQueue;
};

} // namespace Blainn