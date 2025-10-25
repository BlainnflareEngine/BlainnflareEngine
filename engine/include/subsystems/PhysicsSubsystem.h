#pragma once

#include <cstdint>

#include "EASTL/unordered_map.h"
#include "EASTL/vector.h"

#include "runtime/components/PhysicsComponent.h"
#include "runtime/physics/ContactListenerImpl.h"
#include "runtime/physics/Layers.h"

namespace JPH
{
class JobSystem;
class RayCastResult;
class TempAllocator;
class PhysicsSystem;
} // namespace JPH

namespace Blainn
{
class BodyBuilder;
class BPLayerInterfaceImpl;
class ObjectVsBroadPhaseLayerFilterImpl;

class PhysicsSubsystem
{
public:
    static void Init();
    static void Destroy();

    static void Update(float deltaTimeMs);

    // TODO: Additional physics-specific methods can be added here
    // createShape(shape type, position, optional params) -> returns shape ID
    // applyForce(shape ID, force vector)
    // add velocity and so on

    // TODO: shape position rotation motion_type object_layer
    // static uuid QueuePhysicsComponentCreation(uuid parentId);
    // static uuid QueuePhysicsComponentCreation(uuid parentId, const BodyBuilder& builder);

    // TODO: shape position rotation motion_type object_layer
    // static uuid CreatePhysicsComponent(uuid parentId);
    // static uuid CreatePhysicsComponent(uuid parentId, const BodyBuilder& builder);

    // TODO: create multipleBodies?

    // JPH::RayCastResult CastRay(Vec3 origin, Vec3 direction);

    static JPH::PhysicsSystem &GetPhysicsSystem();

private:
    PhysicsSubsystem() = delete;
    PhysicsSubsystem(const PhysicsSubsystem &) = delete;
    PhysicsSubsystem &operator=(const PhysicsSubsystem &) = delete;
    PhysicsSubsystem(const PhysicsSubsystem &&) = delete;
    PhysicsSubsystem &operator=(const PhysicsSubsystem &&) = delete;

    inline static constexpr float m_physicsUpdatePeriodMs = 1000.0 / 120.0; // 120 Hz

    inline static bool m_isInitialized = false;

    static eastl::unordered_map<uuid, PhysicsComponent> m_physicsComponents;
    static eastl::vector<eastl::pair<uuid, PhysicsComponent>> m_physicsComponentCreationQueue;

    inline static constexpr uint32_t m_maxConcurrentJobs = 8;
    inline static eastl::unique_ptr<JPH::JobSystem> m_joltJobSystem = nullptr;
    inline static eastl::unique_ptr<JPH::TempAllocator> m_joltTempAllocator = nullptr;
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