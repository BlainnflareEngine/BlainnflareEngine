#pragma once

namespace Blainn {
	class PhysicsSubsystem  {
	public:
		static PhysicsSubsystem& GetInstance();

		static void Init();
		static void Destroy();
		// TODO: Additional physics-specific methods can be added here
		// createShape(shape type, position, optional params) -> returns shape ID
		// applyForce(shape ID, force vector)
		// add velocity and so on
	private:
        PhysicsSubsystem() = delete; 
        PhysicsSubsystem(const PhysicsSubsystem&) = delete;
        PhysicsSubsystem& operator=(const PhysicsSubsystem&) = delete; 
		PhysicsSubsystem(const PhysicsSubsystem&&) = delete;
        PhysicsSubsystem& operator=(const PhysicsSubsystem&&) = delete; 

		static bool m_isInitialized;
	};
}