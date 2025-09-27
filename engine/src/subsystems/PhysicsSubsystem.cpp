#include "subsystems/PhysicsSubsystem.h"

#include <cassert>

#include "Jolt/Jolt.h"
#include "Jolt/Core/TempAllocator.h"
#include "Jolt/Core/JobSystemThreadPool.h"
#include "Jolt/Physics/PhysicsSettings.h"
#include "Jolt/Core/JobSystemSingleThreaded.h"
#include "Jolt/Physics/PhysicsSystem.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhase.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayer.h"
#include "Jolt/Physics/Collision/BroadPhase/BroadPhaseLayerInterfaceMask.h"
#include "Jolt/Physics/Collision/ContactListener.h"
#include "Jolt/Physics/Body/BodyCreationSettings.h"

#include "runtime/physics/Layers.h"
#include "runtime/physics/BodyBuilder.h"
#include "runtime/physics/ContactListenerImpl.h"

using namespace Blainn;

void PhysicsSubsystem::Init() {
    m_isInitialized = true;

    // TODO: set simulation frequency

	m_joltTempAllocator = eastl::make_unique<JPH::TempAllocatorImpl>(32 * 1024 * 1024);
	m_joltJobSystem = eastl::make_unique<JPH::JobSystemThreadPool>(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, m_maxConcurrentJobs - 1);
    // Create physics system
    m_joltPhysicsSystem = eastl::make_unique<JPH::PhysicsSystem>();

    m_broadPhaseLayerInterface = eastl::make_unique<BPLayerInterfaceImpl>();
    m_objectVsBroadPhaseLayerFilter = eastl::make_unique<ObjectVsBroadPhaseLayerFilterImpl>();
    m_objectVsObjectLayerFilter = eastl::make_unique<ObjectLayerPairFilterImpl>();

    JPH::PhysicsSettings mPhysicsSettings;	
	m_joltPhysicsSystem->Init(cNumBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, *m_broadPhaseLayerInterface, *m_objectVsBroadPhaseLayerFilter, *m_objectVsObjectLayerFilter);
	m_joltPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);

    m_contactListener = eastl::make_unique<ContactListenerImpl>();
	// Optimize the broadphase to make the first update fast
	m_joltPhysicsSystem->OptimizeBroadPhase();

    // TODO: reserve m_bodyCreationQueue and m_PhysicsComponents
}

void PhysicsSubsystem::Destroy() {
}

void PhysicsSubsystem::DoCycle()
{
    // TODO: copy body properties to transforms

    // TODO: create queued bodies
}

uuid PhysicsSubsystem::QueuePhysicsComponentCreation(uuid parentId)
{
    JPH::BodyCreationSettings settings;
    
    // TODO: fill settings.
    JPH::BodyInterface().CreateBody(settings);
}

uuid Blainn::PhysicsSubsystem::QueuePhysicsComponentCreation(uuid parentId, const BodyBuilder &builder)
{
    

    //TODO: create physics component

    
}

JPH::RayCastResult Blainn::PhysicsSubsystem::CastRay(JPH::Vec3 origin, JPH::Vec3 direction)
{
    //TODO: implement
    // TODO: probaply forward to shape (physics component)
}