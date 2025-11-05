#pragma once

#include <cstdint>

#include <EASTL/optional.h>
#include <EASTL/unique_ptr.h>
#include <EASTL/unordered_map.h>
#include <EASTL/vector.h>

#include <Jolt/Jolt.h>

#include <Jolt/Core/JobSystemSingleThreaded.h>
#include <Jolt/Core/TempAllocator.h>
#include <Jolt/Physics/EActivation.h>
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


struct PhysicsComponentSettings
{
    PhysicsComponentSettings(Entity entityIn, ComponentShapeType shapeTypeIn)
        : entity(entityIn)
        , shapeType(shapeTypeIn)
    {
    }

    Entity entity;
    ComponentShapeType shapeType;
    JPH::EActivation activate = JPH::EActivation::DontActivate;
    PhysicsComponentMotionType motionType = PhysicsComponentMotionType::Dynamic;
    JPH::ObjectLayer layer = 0;
    Vec3 position = Vec3::Zero;
    Quat rotation = Quat::Identity;
    bool isTrigger = false;
    float radius = 0.5f;                       // sphere, capsule, cylinder
    Vec3 halfExtents = Vec3{0.5f, 0.5f, 0.5f}; // box
    float halfCylinderHeight = 0.5f;           // capsule, cylinder
};

class PhysicsSubsystem
{
public:
    static void Init(Timeline<eastl::chrono::milliseconds> &globalTimeline);
    static void Destroy();

    /// @brief physics has its own timeline so you may not pass deltaTime to Update()
    static void Update();

    static void StartSimulation();
    static void StopSimulation();

    // TODO: QueuePhysicsComponentCreation();

    static void CreateComponent(PhysicsComponentSettings &settings);
    static bool HasComponent(Entity entity);
    static void DestroyComponent(Entity entity);

    static void SetVelocity(Entity entity, Vec3 velocity);
    static void SetMaxLinearVelocity(Entity entity, Vec3 maxVelocity);
    static void SetAngularVelocity(Entity entity, Vec3 angularVelocity);
    static void SetMaxAngularVelocity(Entity entity, Vec3 maxAngularVelocity);
    static void SetPosition(Entity entity, Vec3 position);
    static void SetRotation(Entity entity, Quat rotation);
    static void SetGravityFactor(Entity entity, float gravityFactor);
    static void SetObjectLayer(Entity entity, JPH::ObjectLayer layer);

    static void AddVelocity(Entity entity, Vec3 deltaVelocity);
    static void AddAngularVelocity(Entity entity, Vec3 deltaAngularVelocity);
    static void AddImpulse(Entity entity, Vec3 impulse);
    static void AddForce(Entity entity, Vec3 force);

    bool IsBodyActive(Entity entity);

    static void ActivateBody(Entity entity);
    static void DeactivateBody(Entity entity);


    static eastl::optional<RayCastResult> CastRay(Vec3 origin, Vec3 directionAndDistance);

    // TODO: hide somehow?
    static JPH::PhysicsSystem &GetPhysicsSystem();

private:
    PhysicsSubsystem() = delete;

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