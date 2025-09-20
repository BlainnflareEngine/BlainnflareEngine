#pragma once
#include "Subsystem.h"

namespace Blainn {
	// PhysicsSubsystem
	class Jord : public Subsystem {
	public:
		static Jord& GetInstance();

		void Init() override;
		void Destroy() override;
		// TODO: Additional physics-specific methods can be added here
		// createShape(shape type, position, optional params) -> returns shape ID
		// applyForce(shape ID, force vector)
		// add velocity and so on
	private:
        Jord(); 
        Jord(const Jord&) = delete;
        Jord& operator=(const Jord&) = delete; 
		Jord(const Jord&&) = delete;
        Jord& operator=(const Jord&&) = delete; 

		static bool m_isInitialized;
	};
}