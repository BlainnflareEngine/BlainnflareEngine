

// TODO: move to launcher(application) class?

#include "editor/include/Editor.h"
#include "engine/include/Engine.h"

#include <iostream>
int main(int argc, char** argv) {
	// TODO:

#if defined(BLAINN_INCLUDE_EDITOR)
	// init editor
    Blainn::Editor editor(argc, argv);
    editor.Show();
    // TODO:
    // editor.Update() should run in engine?
    // or editor.Update() and Engine::Run() should be updated here in while true
    // to grab HWND from editor just call editor.GetViewportHWND()
    // code below is just for tests
    /*while (true)
    {
        editor.Update();
    }*/
#endif

	Blainn::Engine::Run();

	return 0;
}