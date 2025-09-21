#pragma once

#include "EASTL/unordered_map.h"
#include "EASTL/vector.h"

#include "aliases.h"

namespace Blainn {
	class SomeType; // TODO: remove
	class BodyBuilder;
	class PhysicsComponent;

	// PhysicsSubsystem
	class PhysicsSubsystem  {
	public:
		static void Init();
		static void Destroy();

		static void DoCycle();

		// TODO: Additional physics-specific methods can be added here
		// createShape(shape type, position, optional params) -> returns shape ID
		// applyForce(shape ID, force vector)
		// add velocity and so on

		// TODO: shape position rotation motion_type object_layer
		static uuid QueuePhysicsComponentCreation(uuid parentId);
		static uuid QueuePhysicsComponentCreation(uuid parentId, const BodyBuilder& builder);

		// TODO: shape position rotation motion_type object_layer
		static uuid CreatePhysicsComponent(uuid parentId);
		static uuid CreatePhysicsComponent(uuid parentId, const BodyBuilder& builder);

		//TODO: create multipleBodies?

		JPH::RayCastResult CastRay(JPH::Vec3 origin, JPH::Vec3 direction);

	private:
        PhysicsSubsystem() = delete; 
        PhysicsSubsystem(const PhysicsSubsystem&) = delete;
        PhysicsSubsystem& operator=(const PhysicsSubsystem&) = delete; 
		PhysicsSubsystem(const PhysicsSubsystem&&) = delete;
        PhysicsSubsystem& operator=(const PhysicsSubsystem&&) = delete; 

		inline static bool m_isInitialized = false;

		static eastl::unordered_map<uuid, PhysicsComponent> m_physicsComponents;
		static eastl::vector<eastl::pair<uuid, PhysicsComponent>> m_physicsComponentCreationQueue;
	};
}