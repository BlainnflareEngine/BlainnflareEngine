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
#include "PhysicsSubsystem.h"

using namespace Blainn;

void PhysicsSubsystem::Init() {
    m_isInitialized = true;

    constexpr int joltUpdateFrequencyHz = 120;

	JPH::TempAllocatorImpl::TempAllocator *	mTempAllocator = new JPH::TempAllocatorImpl(32 * 1024 * 1024);

    int mMaxConcurrentJobs = 4;
	JPH::JobSystem * mJobSystem = new JPH::JobSystemThreadPool(JPH::cMaxPhysicsJobs, JPH::cMaxPhysicsBarriers, mMaxConcurrentJobs - 1);

	//JPH::JobSystemSingleThreaded * mJobSystem = new JPH::JobSystemSingleThreaded(JPH::cMaxPhysicsJobs);

    // Create physics system
	JPH::PhysicsSystem* mPhysicsSystem = new JPH::PhysicsSystem();

    static constexpr unsigned int cNumBodies = 10240;
    static constexpr unsigned int cNumBodyMutexes = 0; // Autodetect
    static constexpr unsigned int cMaxBodyPairs = 65536;
    static constexpr unsigned int cMaxContactConstraints = 20480;
    BPLayerInterfaceImpl mBroadPhaseLayerInterface;
    ObjectVsBroadPhaseLayerFilterImpl mObjectVsBroadPhaseLayerFilter;
    ObjectLayerPairFilterImpl mObjectVsObjectLayerFilter;	
    JPH::PhysicsSettings mPhysicsSettings;	
	mPhysicsSystem->Init(cNumBodies, cNumBodyMutexes, cMaxBodyPairs, cMaxContactConstraints, mBroadPhaseLayerInterface, mObjectVsBroadPhaseLayerFilter, mObjectVsObjectLayerFilter);
	mPhysicsSystem->SetPhysicsSettings(mPhysicsSettings);

    ContactListenerImpl * mContactListener = new ContactListenerImpl;

	// Optimize the broadphase to make the first update fast
	mPhysicsSystem->OptimizeBroadPhase();

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