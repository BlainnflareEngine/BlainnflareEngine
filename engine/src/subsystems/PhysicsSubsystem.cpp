#include "pch.h"

#include "subsystems/PhysicsSubsystem.h"

#include "Engine.h"
#include "subsystems/Log.h"

#include "physics/BodyBuilder.h"
#include "physics/ContactListenerImpl.h"
#include "physics/Layers.h"
#include "physics/RayCastResult.h"
#include "physics/ShapeFactory.h"

#include "scene/BasicComponents.h"
#include "scene/Scene.h"
#include "scene/TransformComponent.h"

using namespace Blainn;

void PhysicsSubsystem::Init(Timeline<eastl::chrono::milliseconds> &globalTimeline)
{
    m_physicsTimeline =
        eastl::make_unique<PeriodicTimeline<eastl::chrono::milliseconds>>(m_physicsUpdatePeriodMs, &globalTimeline);

    JPH::RegisterDefaultAllocator();
    m_joltTempAllocator = eastl::make_unique<JPH::TempAllocatorImpl>(32 * 1024 * 1024);

    // can be changed to job system
    m_joltJobSystem = eastl::make_unique<JPH::JobSystemSingleThreaded>(JPH::cMaxPhysicsJobs);

    m_joltPhysicsSystem = eastl::make_unique<JPH::PhysicsSystem>();

    m_factory = eastl::make_unique<JPH::Factory>();
    JPH::Factory::sInstance = m_factory.get();

    m_broadPhaseLayerInterface = eastl::make_unique<BPLayerInterfaceImpl>();
    m_objectVsBroadPhaseLayerFilter = eastl::make_unique<ObjectVsBroadPhaseLayerFilterImpl>();
    m_objectVsObjectLayerFilter = eastl::make_unique<ObjectLayerPairFilterImpl>();

    JPH::PhysicsSettings mPhysicsSettings;

    m_joltPhysicsSystem->Init(cNumBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints,
                              *m_broadPhaseLayerInterface, *m_objectVsBroadPhaseLayerFilter,
                              *m_objectVsObjectLayerFilter);
    m_joltPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);

    m_contactListener = eastl::make_unique<ContactListenerImpl>();
    m_joltPhysicsSystem->SetContactListener(m_contactListener.get());

    JPH::RegisterTypes();

    // Optimize the broadphase to make the first update fast
    m_joltPhysicsSystem->OptimizeBroadPhase();

    m_isInitialized = true;
}

void PhysicsSubsystem::Destroy()
{
    // TODO: ?
}

void PhysicsSubsystem::Update()
{
    assert(m_isInitialized && "PhysicsSubsystem not initialized. Call PhysicsSubsystem::Init() before using it.");

    float deltaTime = m_physicsTimeline->Tick();
    if (deltaTime == 0.0f) return;
    deltaTime /= 1000.0f;

    eastl::shared_ptr<Scene> activeScene = Engine::GetActiveScene();
    auto enities = activeScene->GetAllEntitiesWith<IDComponent, TransformComponent, PhysicsComponent>();

    for (const auto &[_, idComp, transformComp, physicsComp] : enities.each())
    {
        if (!transformComp.IsDirty()) continue;
        UpdateBodyInJolt(activeScene, idComp.ID);
    }

    m_joltPhysicsSystem->Update(deltaTime, physicsUpdateSubsteps, m_joltTempAllocator.get(), m_joltJobSystem.get());

    for (const auto &[_, idComp, transformComp, physicsComp] : enities.each())
    {
        Entity entity = activeScene->GetEntityWithUUID(idComp.ID);

        BodyGetter bodyGetter = GetBodyGetter(entity);
        if (bodyGetter.isTrigger()) continue;

        transformComp.SetTranslation(bodyGetter.GetPosition());
        transformComp.SetRotation(bodyGetter.GetRotation());
        activeScene->SetFromWorldSpaceTransformMatrix(entity, transformComp.GetTransform());
    }

    ProcessEvents();
}

void PhysicsSubsystem::StartSimulation()
{
    m_physicsTimeline->Start();

    eastl::shared_ptr<Scene> activeScene = Engine::GetActiveScene();
    auto entities = activeScene->GetAllEntitiesWith<IDComponent, TransformComponent, PhysicsComponent>();

    for (const auto &[_, idComp, transformComp, physicsComp] : entities.each())
    {
        PhysicsSubsystem::UpdateBodyInJolt(activeScene, idComp.ID);
        Entity entity = activeScene->GetEntityWithUUID(idComp.ID);
        BodyUpdater bodyUpdater = GetBodyUpdater(entity);
        bodyUpdater.ActivateBody();
    }
}


void Blainn::PhysicsSubsystem::UpdateBodyInJolt(const eastl::shared_ptr<Blainn::Scene> &activeScene,
                                                const uuid &entityUuid)
{
    Entity entity = activeScene->GetEntityWithUUID(entityUuid);

    Vec3 translation, scale;
    Quat rotation;
    activeScene->GetWorldSpaceTransformMatrix(entity).Decompose(scale, rotation, translation);

    BodyUpdater bodyUpdater = GetBodyUpdater(entity);
    bodyUpdater.SetPosition(translation).SetRotation(rotation);
}

void PhysicsSubsystem::StopSimulation()
{
    m_physicsTimeline->Pause();

    eastl::shared_ptr<Scene> activeScene = Engine::GetActiveScene();
    auto enities = activeScene->GetAllEntitiesWith<IDComponent, TransformComponent, PhysicsComponent>();

    for (const auto &[_, idComp, transformComp, physicsComp] : enities.each())
    {
        Entity entity = activeScene->GetEntityWithUUID(idComp.ID);
        BodyUpdater bodyUpdater = GetBodyUpdater(entity);
        bodyUpdater.DeactivateBody();
    }
}

void PhysicsSubsystem::CreateAttachPhysicsComponent(PhysicsComponentSettings &settings)
{
    TransformComponent *transformComponentPtr = settings.entity.TryGetComponent<TransformComponent>();
    if (!transformComponentPtr)
    {
        BF_ERROR("Entity must have transform component to create physics component");
        return;
    }

    PhysicsComponent *componentPtr = settings.entity.TryGetComponent<PhysicsComponent>();

    if (componentPtr)
    {
        BF_ERROR("Entity already has physics component");
        return;
    }

    uuid parentId = settings.entity.GetUUID();

    PhysicsComponent component;
    component.parentId = parentId;
    component.prevFrameScale = transformComponentPtr->GetScale();

    eastl::optional<JPH::Ref<JPH::Shape>> createdShapeHierarchy = ShapeFactory::CreateShape(settings.shapeSettings);

    if (!createdShapeHierarchy.has_value())
    {
        BF_ERROR("Error in creating shape for physics component");
        return;
    }
    component.UpdateShape(settings.shapeSettings.shapeType, createdShapeHierarchy.value());


    BodyBuilder builder;
    builder.SetMotionType(settings.motionType)
        .SetPosition(transformComponentPtr->GetTranslation())
        .SetRotation(transformComponentPtr->GetRotation())
        .SetShape(component.GetShape().GetPtr())
        .SetIsTrigger(settings.isTrigger)
        .SetGravityFactor(settings.gravityFactor)
        .SetLayer(settings.layer);

    component.bodyId = builder.Build(settings.activate);
    m_bodyEntityConnections.try_emplace(component.bodyId, parentId);

    settings.entity.AddComponent<PhysicsComponent>(eastl::move(component));
}

bool PhysicsSubsystem::HasPhysicsComponent(Entity entity)
{
    return entity.HasComponent<PhysicsComponent>();
}

void PhysicsSubsystem::DestroyPhysicsComponent(Entity entity)
{
    PhysicsComponent *component = entity.TryGetComponent<PhysicsComponent>();
    if (!component) return;

    JPH::BodyInterface &bodyInterface = m_joltPhysicsSystem->GetBodyInterface();
    bodyInterface.RemoveBody(component->bodyId);
    bodyInterface.DestroyBody(component->bodyId);
    m_bodyEntityConnections.erase(component->bodyId);
    entity.RemoveComponent<PhysicsComponent>();
}

JPH::BodyID PhysicsSubsystem::GetBodyId(Entity entity)
{
    PhysicsComponent &component = entity.GetComponent<PhysicsComponent>();
    return component.bodyId;
}


eastl::optional<Entity> Blainn::PhysicsSubsystem::GetEntityByBodyId(JPH::BodyID bodyId)
{
    if (!m_bodyEntityConnections.contains(bodyId)) [[unlikely]]
    {
        BF_ERROR("can not get entity by body id {} - does not exist", bodyId.GetIndexAndSequenceNumber());
        return eastl::optional<Entity>{};
    }

    return eastl::optional<Entity>(Engine::GetActiveScene()->GetEntityWithUUID(m_bodyEntityConnections[bodyId]));
}

PhysicsEventHandle PhysicsSubsystem::AddEventListener(
    const PhysicsEventType eventType, eastl::function<void(const eastl::shared_ptr<PhysicsEvent> &)> listener)
{
    return s_physicsEventQueue.appendListener(eventType, listener);
}

void PhysicsSubsystem::RemoveEventListener(const PhysicsEventType eventType, const PhysicsEventHandle &handle)
{
    s_physicsEventQueue.removeListener(eventType, handle);
}

JPH::PhysicsSystem &PhysicsSubsystem::GetPhysicsSystem()
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
    JPH::BodyID hitBodyId = joltResult.mBodyID;
    BodyGetter bodyGetter(m_joltPhysicsSystem->GetBodyLockInterface(), m_joltPhysicsSystem->GetBodyInterface(),
                          hitBodyId);
    Vec3 bodyPosition = bodyGetter.GetPosition();

    RayCastResult rayCastResult;
    rayCastResult.entityId = m_bodyEntityConnections[hitBodyId];
    rayCastResult.distance = joltResult.mFraction * directionAndDistance.Length();
    rayCastResult.hitPoint = origin + directionAndDistance * joltResult.mFraction;

    // TODO: get child shape or leave this?
    JPH::RefConst<JPH::Shape> bodyShape = bodyGetter.GetShape();
    rayCastResult.hitNormal = ToBlainnVec3(
        bodyShape->GetSurfaceNormal(joltResult.mSubShapeID2, ToJoltVec3(rayCastResult.hitPoint - bodyPosition)));

    return eastl::optional<RayCastResult>(eastl::move(rayCastResult));
}


void Blainn::PhysicsSubsystem::ProcessEvents()
{
    s_physicsEventQueue.process();
}
PhysicsComponent &Blainn::PhysicsSubsystem::GetPhysicsComponentByBodyId(JPH::BodyID bodyId)
{
    assert(m_bodyEntityConnections.contains(bodyId));
    return Engine::GetActiveScene()
        ->GetEntityWithUUID(m_bodyEntityConnections.at(bodyId))
        .GetComponent<PhysicsComponent>();
}

bool PhysicsSubsystem::IsBodyActive(Entity entity)
{
    PhysicsComponent &component = entity.GetComponent<PhysicsComponent>();
    return m_joltPhysicsSystem->GetBodyInterface().IsActive(component.bodyId);
}

void PhysicsSubsystem::ActivateBody(Entity entity)
{
    PhysicsComponent &component = entity.GetComponent<PhysicsComponent>();
    m_joltPhysicsSystem->GetBodyInterface().ActivateBody(component.bodyId);
}

void PhysicsSubsystem::DeactivateBody(Entity entity)
{
    PhysicsComponent &component = entity.GetComponent<PhysicsComponent>();
    m_joltPhysicsSystem->GetBodyInterface().DeactivateBody(component.bodyId);
}

BodyUpdater PhysicsSubsystem::GetBodyUpdater(Entity entity)
{
    PhysicsComponent &component = entity.GetComponent<PhysicsComponent>();
    return BodyUpdater(m_joltPhysicsSystem->GetBodyLockInterface(), m_joltPhysicsSystem->GetBodyInterface(),
                       component.bodyId);
}

BodyGetter PhysicsSubsystem::GetBodyGetter(Entity entity)
{
    PhysicsComponent &component = entity.GetComponent<PhysicsComponent>();
    return BodyGetter(m_joltPhysicsSystem->GetBodyLockInterface(), m_joltPhysicsSystem->GetBodyInterface(),
                      component.bodyId);
}
