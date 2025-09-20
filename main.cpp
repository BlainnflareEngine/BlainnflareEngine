

// TODO: move to launcher(application) class?

#include "engine/include/Engine.h"


#include <iostream>
int main(int argc, char** argv) {
	// TODO:
	
	Blainn::Engine::Init();
#if defined(BLAINN_INCLUDE_EDITOR)
	// init editor
#endif
	Blainn::Engine::Run();

	return 0;
}