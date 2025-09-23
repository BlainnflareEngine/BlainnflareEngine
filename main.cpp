

// TODO: move to launcher(application) class?

#include "editor/include/Editor.h"
#include "engine/include/Engine.h"
#include "engine/include/subsystems/Freya.h"

#include <cstdint>
#include <iostream>

void *__cdecl operator new[](size_t size, const char *name, int flags, unsigned debugFlags, const char *file, int line)
{
    return new uint8_t[size];
}


int main(int argc, char **argv)
{
    // TODO:

    Blainn::Freya::Init();
#if defined(BLAINN_INCLUDE_EDITOR)
    // init editor
    Blainn::Editor editor(argc, argv);
    editor.Show();
    // TODO:
    // editor.Update() should run in engine?
    // or editor.Update() and Engine::Run() should be updated here in while true
    // to grab HWND from editor just call editor.GetViewportHWND()
    // code below is just for tests
    while (true)
    {
        editor.Update();
    }
#endif

    Blainn::Engine::Run();

    return 0;
}