#pragma once

#include "Subsystem.h"

namespace Blainn
{
	// TODO:
	class RenderSubsystem : public Subsystem
	{
	public:
		//TODO: virtual ~RenderSubsystem() override = default;
		virtual void Init() override;
		virtual void Destroy() override;
		// TODO: Additional rendering-specific methods can be added here
	};
}