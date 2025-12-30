#pragma once

#include "components/PhysicsComponent.h"
#include "physics/BodyGetter.h"
#include "physics/BodyUpdater.h"
#include "physics/ContactListenerImpl.h"
#include "physics/Layers.h"
#include "physics/PhysicsEvents.h"
#include "physics/PhysicsTypes.h"
#include "physics/PhysicsCreationSettings.h"
#include "scene/Scene.h"
#include "tools/PeriodicTimeline.h"


namespace JPH
{
class JobSystem;
class TempAllocator;
} // namespace JPH

namespace Blainn
{
class BodyBuilder;
class BPLayerInterfaceImpl;
class ObjectVsBroadPhaseLayerFilterImpl;
class RayCastResult;

class PhysicsSubsystem
{
public:
    static void Init(Timeline<eastl::chrono::milliseconds> &globalTimeline);
    static void Destroy();

    /// @brief physics has its own timeline so you do not need to pass deltaTime in Update()
    static void Update();

    static void StartSimulation();
    static void StopSimulation();

    // TODO: QueuePhysicsComponentCreation()?;

    static void CreateAttachPhysicsComponent(PhysicsComponentSettings &settings);
    static bool HasPhysicsComponent(Entity entity);
    static void DestroyPhysicsComponent(Entity entity);

    static JPH::BodyID GetBodyId(Entity entity);
    static eastl::optional<Entity> GetEntityByBodyId(JPH::BodyID bodyId);

    /// @brief does not check entity or component exist! You are warned.
    static PhysicsComponent &GetPhysicsComponentByBodyId(JPH::BodyID bodyId);

    static bool IsBodyActive(Entity entity);
    static void ActivateBody(Entity entity);
    static void DeactivateBody(Entity entity);

    static BodyUpdater GetBodyUpdater(Entity entity);
    static BodyGetter GetBodyGetter(Entity entity);

    static eastl::optional<RayCastResult> CastRay(Vec3 origin, Vec3 directionAndDistance);

    using PhysicsEventHandle =
        eventpp::internal_::CallbackListBase<void(const eastl::shared_ptr<PhysicsEvent> &), PhysicsEventPolicy>::Handle;
    static PhysicsEventHandle AddEventListener(const PhysicsEventType eventType,
                                               eastl::function<void(const eastl::shared_ptr<PhysicsEvent> &)> listener);
    static void RemoveEventListener(const PhysicsEventType eventType, const PhysicsEventHandle &handle);

    // TODO: hide somehow?
    static JPH::PhysicsSystem &GetPhysicsSystem();

    static void AddBodyConnection(JPH::BodyID bodyID, uuid parentID);

private:
    PhysicsSubsystem() = delete;

    static void ProcessEvents();

    inline static eventpp::EventQueue<PhysicsEventType, void(const eastl::shared_ptr<PhysicsEvent> &),
                                      PhysicsEventPolicy>
        s_physicsEventQueue;

    inline static const int physicsUpdateFrequency = 60;
    inline static const int physicsUpdateSubsteps = 1;
    inline static const float m_physicsUpdatePeriodMs = 1000.0 / static_cast<float>(physicsUpdateFrequency);
    inline static eastl::unique_ptr<Blainn::PeriodicTimeline<eastl::chrono::milliseconds>> m_physicsTimeline =
        nullptr; // initialized in Init()

    inline static bool m_isInitialized = false;

    inline static eastl::unordered_map<JPH::BodyID, uuid> m_bodyEntityConnections{};

    inline static constexpr uint32_t m_maxConcurrentJobs = 8;
    inline static eastl::unique_ptr<JPH::JobSystemSingleThreaded> m_joltJobSystem = nullptr;
    inline static eastl::unique_ptr<JPH::TempAllocatorImpl> m_joltTempAllocator = nullptr;
    inline static eastl::unique_ptr<JPH::PhysicsSystem> m_joltPhysicsSystem = nullptr;
    inline static eastl::unique_ptr<JPH::Factory> m_factory = nullptr;

    inline static eastl::unique_ptr<BPLayerInterfaceImpl> m_broadPhaseLayerInterface = nullptr;
    inline static eastl::unique_ptr<ObjectVsBroadPhaseLayerFilterImpl> m_objectVsBroadPhaseLayerFilter = nullptr;
    inline static eastl::unique_ptr<ObjectLayerPairFilterImpl> m_objectVsObjectLayerFilter = nullptr;
    inline static eastl::unique_ptr<ContactListenerImpl> m_contactListener = nullptr;

    inline static constexpr uint32_t cNumBodies = 10240;
    inline static constexpr uint32_t cNumBodyMutexes = 0; // Autodetect
    inline static constexpr uint32_t cMaxBodyPairs = 65536;
    inline static constexpr uint32_t cMaxContactConstraints = 20480;

    friend class ContactListenerImpl;
};
} // namespace Blainn