#pragma once

namespace Blainn {
	// PhysicsSubsystem
	class Jord  {
	public:
		static Jord& GetInstance();

		static void Init();
		static void Destroy();
		// TODO: Additional physics-specific methods can be added here
		// createShape(shape type, position, optional params) -> returns shape ID
		// applyForce(shape ID, force vector)
		// add velocity and so on
	private:
        Jord() = delete; 
        Jord(const Jord&) = delete;
        Jord& operator=(const Jord&) = delete; 
		Jord(const Jord&&) = delete;
        Jord& operator=(const Jord&&) = delete; 

		static bool m_isInitialized;
	};
}