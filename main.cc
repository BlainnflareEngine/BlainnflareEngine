

// TODO: move to launcher(application) class?

#include "engine/include/subsystems/RenderSubsystem.h"

#include <iostream>
int main(int argc, char** argv) {
	// TODO:
	
	// init engine
#if defined(BLAINN_INCLUDE_EDITOR)
	// init editor
#endif

    // just to check that cmake is working
    Blainn::RenderSubsystem subsystem;
    subsystem.Init();
	// run engine

	return 0;
}