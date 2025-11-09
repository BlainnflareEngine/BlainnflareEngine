#include "subsystems/PhysicsSubsystem.h"

#include <cassert>

#include <Jolt/Jolt.h>
#include <Jolt/Physics/Body/Body.h>
#include <Jolt/Physics/Body/BodyCreationSettings.h>
#include <Jolt/Physics/Body/BodyManager.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhase.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h>
#include <Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceMask.h>
#include <Jolt/Physics/Collision/CastResult.h>
#include <Jolt/Physics/Collision/ContactListener.h>
#include <Jolt/Physics/Collision/NarrowPhaseQuery.h>
#include <Jolt/Physics/Collision/RayCast.h>
#include <Jolt/Physics/PhysicsSettings.h>
#include <Jolt/Physics/PhysicsSystem.h>

#include "Engine.h"
#include "PhysicsSubsystem.h"
#include "physics/BodyBuilder.h"
#include "physics/ContactListenerImpl.h"
#include "physics/Layers.h"
#include "physics/RayCastResult.h"
#include "physics/ShapeFactory.h"
#include "scene/BasicComponents.h"
#include "scene/Scene.h"
#include "scene/TransformComponent.h"
#include "subsystems/Log.h"

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
    // TODO:
}

void PhysicsSubsystem::Update()
{
    assert(m_isInitialized && "PhysicsSubsystem not initialized. Call PhysicsSubsystem::Init() before using it.");

    float deltaTimeMs = m_physicsTimeline->Tick();
    if (deltaTimeMs == 0.0f) return;

    Scene &activeScene = Engine::GetActiveScene();
    auto enities = activeScene.GetAllEntitiesWith<IDComponent, TransformComponent, PhysicsComponent>();

    for (auto entityComponents : enities.each())
    {
        const IDComponent &idComp = std::get<1>(entityComponents);
        Entity entity = activeScene.GetEntityWithUUID(idComp.ID);

        Vec3 translation, scale;
        Quat rotation;
        activeScene.GetWorldSpaceTransformMatrix(entity).Decompose(scale, rotation, translation);

        BodyUpdater bodyUpdater = GetBodyUpdater(entity);
        bodyUpdater.SetPosition(translation).SetRotation(rotation).SetScale(scale);
    }

    m_joltPhysicsSystem->Update(deltaTimeMs, 1, m_joltTempAllocator.get(), m_joltJobSystem.get());

    for (auto entityComponents : enities.each())
    {
        const IDComponent &idComp = std::get<1>(entityComponents);
        TransformComponent &transformComp = std::get<2>(entityComponents);
        Entity entity = activeScene.GetEntityWithUUID(idComp.ID);

        BodyGetter bodyGetter = GetBodyGetter(entity);
        transformComp.Translation = bodyGetter.GetPosition();
        transformComp.SetRotation(bodyGetter.GetRotation());
        activeScene.SetFromWorldSpaceTransformMatrix(entity, transformComp.GetTransform());
    }
}

void Blainn::PhysicsSubsystem::StartSimulation()
{
    m_physicsTimeline->Start();
}

void Blainn::PhysicsSubsystem::StopSimulation()
{
    m_physicsTimeline->Pause();
}

void Blainn::PhysicsSubsystem::CreateComponent(PhysicsComponentSettings &settings)
{
    PhysicsComponent *componentPtr = settings.entity.TryGetComponent<PhysicsComponent>();
    assert(!componentPtr && "Entity already has physics component on creation");

    uuid parentId = settings.entity.GetUUID();

    PhysicsComponent component;
    component.parentId = parentId;
    component.shapeType = settings.shapeType;

    switch (settings.shapeType)
    {
    case ComponentShapeType::Sphere:
        component.shapeHierarchy = ShapeFactory::CreateSphereShape(settings.radius);
        break;
    case ComponentShapeType::Box:
        component.shapeHierarchy = ShapeFactory::CreateBoxShape(settings.halfExtents);
        break;
    case ComponentShapeType::Capsule:
        component.shapeHierarchy = ShapeFactory::CreateCapsuleShape(settings.halfCylinderHeight, settings.radius);
        break;
    case ComponentShapeType::Cylinder:
        component.shapeHierarchy = ShapeFactory::CreateCylinderShape(settings.halfCylinderHeight, settings.radius);
    default:
        BF_ERROR("Invalid physics shape type");
        return;
    }

    BodyBuilder builder;
    builder.SetMotionType(settings.motionType);
    builder.SetPosition(settings.position);
    builder.SetRotation(settings.rotation);
    builder.SetShape(component.shapeHierarchy.shapePtr.get());
    builder.SetIsTrigger(settings.isTrigger);

    component.bodyId = builder.Build(settings.activate);
    m_bodyEntityConnections.try_emplace(component.bodyId, parentId);

    settings.entity.AddComponent<PhysicsComponent>(eastl::move(component));
}

bool Blainn::PhysicsSubsystem::HasComponent(Entity entity)
{
    return entity.HasComponent<PhysicsComponent>();
}

void Blainn::PhysicsSubsystem::DestroyComponent(Entity entity)
{
    PhysicsComponent &component = entity.GetComponent<PhysicsComponent>();
    JPH::BodyInterface &bodyInterface = m_joltPhysicsSystem->GetBodyInterface();
    bodyInterface.RemoveBody(component.bodyId);
    bodyInterface.DestroyBody(component.bodyId);
    m_bodyEntityConnections.erase(component.bodyId);
    entity.RemoveComponent<PhysicsComponent>();
}

JPH::BodyID Blainn::PhysicsSubsystem::GetBodyId(Entity entity)
{
    PhysicsComponent &component = entity.GetComponent<PhysicsComponent>();
    return component.bodyId;
}


JPH::PhysicsSystem &Blainn::PhysicsSubsystem::GetPhysicsSystem()
{
    return *m_joltPhysicsSystem;
}

eastl::optional<RayCastResult> PhysicsSubsystem::CastRay(Vec3 origin, Vec3 directionAndDistance)
{
    JPH::RRayCast ray(ToJoltRVec3(origin), ToJoltRVec3(directionAndDistance));
    JPH::RayCastResult joltResult;
    if (!m_joltPhysicsSystem->GetNarrowPhaseQuery().CastRay(ray, joltResult))
    {
        return eastl::optional<RayCastResult>();
    }

    RayCastResult rayCastResult;
    rayCastResult.entityId = m_bodyEntityConnections[joltResult.mBodyID];
    rayCastResult.distance = joltResult.mFraction * directionAndDistance.Length();
    rayCastResult.hitPoint = origin + directionAndDistance * joltResult.mFraction;
    JPH::RefConst<JPH::Shape> bodyShape = m_joltPhysicsSystem->GetBodyInterface().GetShape(joltResult.mBodyID);
    rayCastResult.hitNormal = ToBlainnVec3(bodyShape->GetSurfaceNormal(
        joltResult.mSubShapeID2,
        ToJoltVec3(rayCastResult.hitPoint) - bodyShape->GetCenterOfMass())); // TODO: convert to world space

    return eastl::optional<RayCastResult>(eastl::move(rayCastResult));
}

bool Blainn::PhysicsSubsystem::IsBodyActive(Entity entity)
{
    PhysicsComponent &component = entity.GetComponent<PhysicsComponent>();
    return m_joltPhysicsSystem->GetBodyInterface().IsActive(component.bodyId);
}

void Blainn::PhysicsSubsystem::ActivateBody(Entity entity)
{
    PhysicsComponent &component = entity.GetComponent<PhysicsComponent>();
    m_joltPhysicsSystem->GetBodyInterface().ActivateBody(component.bodyId);
}

void Blainn::PhysicsSubsystem::DeactivateBody(Entity entity)
{
    PhysicsComponent &component = entity.GetComponent<PhysicsComponent>();
    m_joltPhysicsSystem->GetBodyInterface().DeactivateBody(component.bodyId);
}

BodyUpdater Blainn::PhysicsSubsystem::GetBodyUpdater(Entity entity)
{
    PhysicsComponent &component = entity.GetComponent<PhysicsComponent>();
    return BodyUpdater(m_joltPhysicsSystem->GetBodyLockInterface(), m_joltPhysicsSystem->GetBodyInterface(),
                       component.bodyId);
}

BodyGetter Blainn::PhysicsSubsystem::GetBodyGetter(Entity entity)
{
    PhysicsComponent &component = entity.GetComponent<PhysicsComponent>();
    return BodyGetter(m_joltPhysicsSystem->GetBodyLockInterface(), m_joltPhysicsSystem->GetBodyInterface(),
                      component.bodyId);
}
