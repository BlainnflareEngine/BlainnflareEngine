#pragma once
#include "Subsystem.h"

namespace Blainn {
	// TODO:
	class PhysicsSubsystem : public Subsystem {
	public:
		void Init() override;
		void Destroy() override;
		// TODO: Additional physics-specific methods can be added here
		// createShape(shape type, position, optional params) -> returns shape ID
		// applyForce(shape ID, force vector)
		// add velocity and so on
	};
}