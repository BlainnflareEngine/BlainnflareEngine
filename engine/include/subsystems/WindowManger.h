#pragma once

#include "Subsystem.h"

namespace Blainn {
	// TODO:
	class WindowManager : public Subsystem {
	public:
		void Init() override;
		void Destroy() override;
		// get window handle (to use in rendering and editor window)
	};
}