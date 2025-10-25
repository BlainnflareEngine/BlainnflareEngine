

// TODO: move to launcher(application) class?

#include "AssetManager.h"
#include "editor/include/Editor.h"
#include "engine/include/Engine.h"
#include "engine/include/subsystems/RenderSubsystem.h"
#include <QApplication>
#include <QPushButton>
#include <pch.h>

#include <iostream>
int main(int argc, char **argv)
{
    // TODO:
    Blainn::Log::Init();

    Blainn::AssetManager::GetInstance().Init();
    auto a = Blainn::AssetManager::GetInstance().LoadTexture(std::filesystem::current_path(), Blainn::TextureType::ALBEDO);

#if defined(BLAINN_INCLUDE_EDITOR)
    // needed for qt to generate resources (icons etc.)
    Q_INIT_RESOURCE(sources);

    // init editor
    Blainn::Editor::GetInstance().Init(argc, argv);
    Blainn::Editor::GetInstance().Show();

    BF_DEBUG("This is debug!");
    BF_ERROR("This is error!");
    BF_INFO("This is info!");
    BF_WARN("This is warn!");
    BF_TRACE("This is trace!");
    // TODO:
    // editor.Update() should run in engine?
    // or editor.Update() and Engine::Run() should be updated here in while true
    // to grab HWND from editor just call editor.GetViewportHWND()
    // code below is just for tests
    while (true)
    {
        Blainn::Editor::GetInstance().Update();
    }
#endif
    Blainn::Engine::Init();
    Blainn::Engine::Run();

    Blainn::Log::Destroy();

    return 0;
}