

// TODO: move to launcher(application) class?

#include "engine/include/Engine.h"


#include <iostream>
int main(int argc, char** argv) {
	// TODO:
	
	Engine::Initialize();
#if defined(BLAINN_INCLUDE_EDITOR)
	// init editor
#endif
	Engine::Run();

	return 0;
}