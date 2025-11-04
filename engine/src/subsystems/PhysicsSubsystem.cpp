#include "subsystems/PhysicsSubsystem.h"

#include <cassert>


#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhase.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceMask.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Collision/NarrowPhaseQuery.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "PhysicsSubsystem.h"
#include "physics/BodyBuilder.h"
#include "physics/ContactListenerImpl.h"
#include "physics/Layers.h"
#include "physics/RayCastResult.h"
#include "physics/ShapeFactory.h"
#include "scene/Scene.h"

using namespace Blainn;

void PhysicsSubsystem::Init(Timeline<eastl::chrono::milliseconds> &globalTimeline)
{
    m_physicsTimeline =
        eastl::make_unique<PeriodicTimeline<eastl::chrono::milliseconds>>(m_physicsUpdatePeriodMs, &globalTimeline);

    JPH::RegisterDefaultAllocator();
    m_joltTempAllocator = eastl::make_unique<JPH::TempAllocatorImpl>(32 * 1024 * 1024);

    // TODO: change to job system
    m_joltJobSystem = eastl::make_unique<JPH::JobSystemSingleThreaded>(JPH::cMaxPhysicsJobs);
    // Create physics system
    m_joltPhysicsSystem = eastl::make_unique<JPH::PhysicsSystem>();

    m_broadPhaseLayerInterface = eastl::make_unique<BPLayerInterfaceImpl>();
    m_objectVsBroadPhaseLayerFilter = eastl::make_unique<ObjectVsBroadPhaseLayerFilterImpl>();
    m_objectVsObjectLayerFilter = eastl::make_unique<ObjectLayerPairFilterImpl>();

    JPH::PhysicsSettings mPhysicsSettings;

    m_joltPhysicsSystem->Init(cNumBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
                              *m_broadPhaseLayerInterface, *m_objectVsBroadPhaseLayerFilter,
                              *m_objectVsObjectLayerFilter);
    m_joltPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);

    m_contactListener = eastl::make_unique<ContactListenerImpl>();
    // Optimize the broadphase to make the first update fast
    m_joltPhysicsSystem->OptimizeBroadPhase();

    // TODO: reserve m_bodyCreationQueue and m_PhysicsComponents

    m_isInitialized = true;
}

void PhysicsSubsystem::Destroy()
{
}

void PhysicsSubsystem::Update()
{
    assert(m_isInitialized && "PhysicsSubsystem not initialized. Call PhysicsSubsystem::Init() before using it.");

    float deltaTimeMs = m_physicsTimeline->Tick();
    if (deltaTimeMs == 0.0f) return;

    static int c;
    std::cout << std::format("physics update {}\n", c);
    ++c;

    m_joltPhysicsSystem->Update(deltaTimeMs, 1, m_joltTempAllocator.get(), m_joltJobSystem.get());

    // TODO: copy body properties to transforms
    // lock no lock all bodies
    // for each body that has a physics component and transform component
    //   get body position and rotation and set transform position and rotation
    // entt::registry &registry = Blainn::GetRegistry();
    // auto view = registry.group<PhysicsComponent>();
    // for (auto entity : view)
    //{
    // PhysicsComponent &physComp = view.get<PhysicsComponent>(entity);
    // TODO: do something?
    //}
    //
    // TODO: create queued bodies
}

void Blainn::PhysicsSubsystem::CreateSpherePhysicsComponent(Entity entity, PhysicsComponentMotionType motionType,
                                                            float radius, Vec3 position, Quat rotation)
{
    CreateAddComponent(entity, motionType, ShapeFactory::CreateSphereShape(radius).second, position, rotation);
}


// uuid PhysicsSubsystem::QueuePhysicsComponentCreation(uuid parentId)
// {
//     JPH::BodyCreationSettings settings;
//     // TODO: fill settings.
//     JPH::BodyInterface().CreateBody(settings);
// }

// uuid Blainn::PhysicsSubsystem::QueuePhysicsComponentCreation(uuid parentId, const BodyBuilder &builder)
// {
//     //TODO: create physics component
// }

JPH::PhysicsSystem &Blainn::PhysicsSubsystem::GetPhysicsSystem()
{
    return *m_joltPhysicsSystem;
}

void Blainn::PhysicsSubsystem::CreateAddComponent(Entity entity, PhysicsComponentMotionType motionType,
                                                  eastl::shared_ptr<JPH::Shape> shapePtr, Vec3 position, Quat rotation)
{
    PhysicsComponent *componentPtr = entity.TryGetComponent<PhysicsComponent>();
    if (componentPtr) return;

    BodyBuilder builder;
    builder.SetMotionType(motionType);
    builder.SetPosition(position);
    builder.SetRotation(rotation);
    builder.SetShape(shapePtr);

    PhysicsComponent comomponent;
    comomponent.m_bodyId = builder.Build();
    comomponent.m_parentId = entity.GetUUID();

    entity.AddComponent<PhysicsComponent>(eastl::move(comomponent));
}

eastl::optional<RayCastResult> PhysicsSubsystem::CastRay(Vec3 origin, Vec3 directionAndDistance)
{
    JPH::RRayCast ray(ToJoltRVec3(origin), ToJoltRVec3(directionAndDistance));
    JPH::RayCastResult result;
    if (!m_joltPhysicsSystem->GetNarrowPhaseQuery().CastRay(ray, result))
    {
        return eastl::optional<RayCastResult>();
    }

    RayCastResult rayCastResult;
    rayCastResult.bodyId; //= bodyUuid;    // TODO: get uuid from body user data
    rayCastResult.distance = result.mFraction * directionAndDistance.Length();
    rayCastResult.hitPoint = origin + directionAndDistance * result.mFraction;
    JPH::RefConst<JPH::Shape> bodyShape = m_joltPhysicsSystem->GetBodyInterface().GetShape(result.mBodyID);
    rayCastResult.hitNormal = ToBlainnVec3(bodyShape->GetSurfaceNormal(
        result.mSubShapeID2,
        ToJoltVec3(rayCastResult.hitPoint) - bodyShape->GetCenterOfMass())); // TODO: convert to world space

    return eastl::optional<RayCastResult>(rayCastResult);
}