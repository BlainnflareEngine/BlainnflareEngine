#pragma once

#include <cstdint>

#include <EASTL/optional.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/unordered_map.h>
#include <EASTL/vector.h>

#include <Jolt/Jolt.h>

#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "components/PhysicsComponent.h"
#include "physics/ContactListenerImpl.h"
#include "physics/Layers.h"
#include "physics/PhysicsTypes.h"
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

    static void Update();


    // TODO: shape position rotation motion_type object_layer
    // static uuid QueuePhysicsComponentCreation(uuid parentId);
    // static uuid QueuePhysicsComponentCreation(uuid parentId, const BodyBuilder& builder);

    // TODO: shape position rotation motion_type object_layer
    static void CreateSpherePhysicsComponent(Entity entity, PhysicsComponentMotionType motionType, float radius,
                                             Vec3 position = Vec3::Zero, Quat rotation = Quat::Identity);
    static void CreatePhysicsComponent(Entity entity, const BodyBuilder &builder);

    static void setVelocity(Entity entity, Vec3 velocity);
    // TODO: Additional physics-specific methods can be added here
    // applyForce(shape ID, force vector)
    // add velocity and so on

    static eastl::optional<RayCastResult> CastRay(Vec3 origin, Vec3 directionAndDistance);

    // TODO: hide somehow?
    static JPH::PhysicsSystem &GetPhysicsSystem();

private:
    PhysicsSubsystem() = delete;

    static void CreateAddComponent(Entity entity, PhysicsComponentMotionType motionType, eastl::shared_ptr<JPH::Shape>,
                                   Vec3 position, Quat rotation);

    inline static constexpr float m_physicsUpdatePeriodMs = 1000.0 / 120.0; // 120 Hz
    inline static eastl::unique_ptr<Blainn::PeriodicTimeline<eastl::chrono::milliseconds>> m_physicsTimeline =
        nullptr; // initialized in Init()

    inline static bool m_isInitialized = false;

    // static eastl::unordered_map<uuid, PhysicsComponent> m_physicsComponents;
    static eastl::vector<eastl::pair<uuid, PhysicsComponent>> m_physicsComponentCreationQueue;

    inline static constexpr uint32_t m_maxConcurrentJobs = 8;
    inline static eastl::unique_ptr<JPH::JobSystemSingleThreaded> m_joltJobSystem = nullptr;
    inline static eastl::unique_ptr<JPH::TempAllocatorImpl> m_joltTempAllocator = nullptr;
    inline static eastl::unique_ptr<JPH::PhysicsSystem> m_joltPhysicsSystem = nullptr;

    inline static eastl::unique_ptr<BPLayerInterfaceImpl> m_broadPhaseLayerInterface = nullptr;
    inline static eastl::unique_ptr<ObjectVsBroadPhaseLayerFilterImpl> m_objectVsBroadPhaseLayerFilter = nullptr;
    inline static eastl::unique_ptr<ObjectLayerPairFilterImpl> m_objectVsObjectLayerFilter = nullptr;
    inline static eastl::unique_ptr<ContactListenerImpl> m_contactListener = nullptr;

    inline static constexpr uint32_t cNumBodies = 10240;
    inline static constexpr uint32_t cNumBodyMutexes = 0; // Autodetect
    inline static constexpr uint32_t cMaxBodyPairs = 65536;
    inline static constexpr uint32_t cMaxContactConstraints = 20480;
};
} // namespace Blainn