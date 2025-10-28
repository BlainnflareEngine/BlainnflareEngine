#include "subsystems/PhysicsSubsystem.h"

#include <cassert>

#include "Jolt/Core/JobSystemSingleThreaded.h"
#include "Jolt/Core/JobSystemThreadPool.h"
#include "Jolt/Core/TempAllocator.h"
#include "Jolt/Jolt.h"
#include "Jolt/Physics/Body/Body.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhase.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceMask.h"
#include "Jolt/Physics/Collision/CastResult.h"
#include "Jolt/Physics/Collision/ContactListener.h"
#include "Jolt/Physics/Collision/NarrowPhaseQuery.h"
#include "Jolt/Physics/Collision/RayCast.h"
#include "Jolt/Physics/PhysicsSettings.h"
#include "Jolt/Physics/PhysicsSystem.h"

#include "physics/BodyBuilder.h"
#include "physics/ContactListenerImpl.h"
#include "physics/Layers.h"
#include "physics/RayCastResult.h"

using namespace Blainn;

void PhysicsSubsystem::Init()
{
    m_isInitialized = true;

    m_joltTempAllocator = eastl::make_unique<JPH::TempAllocatorImpl>(32 * 1024 * 1024);
    m_joltJobSystem = eastl::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers,
                                                                   m_maxConcurrentJobs - 1);
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
}

void PhysicsSubsystem::Destroy()
{
}

void PhysicsSubsystem::Update(float deltaTimeMs)
{
    assert(m_isInitialized && "PhysicsSubsystem not initialized. Call PhysicsSubsystem::Init() before using it.");
    if (deltaTimeMs == 0.0f) return;

    static float accumulatedTimeMs;
    accumulatedTimeMs += deltaTimeMs;
    while (accumulatedTimeMs > m_physicsUpdatePeriodMs)
    {
        accumulatedTimeMs -= m_physicsUpdatePeriodMs;

        m_joltPhysicsSystem->Update(m_physicsUpdatePeriodMs, 1, m_joltTempAllocator.get(), m_joltJobSystem.get());

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