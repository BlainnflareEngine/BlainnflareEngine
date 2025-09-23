//
// Created by gorev on 21.09.2025.
//

#include "../include/Editor.h"
#include "ui_editor_main.h"
#include <iostream>


namespace Blainn
{
Editor::Editor(int &argc, char **argv)
    : m_app(argc, argv)
    , m_editor_main()
{
    m_editor_main.SetContentDirectory(QDir::currentPath().append("/Content"));
}

void Editor::Show()
{
    std::cout << "Editor::Init()" << std::endl;
    m_editor_main.showMaximized();
}


HWND Editor::GetViewportHWND()
{
    return m_editor_main.GetViewportHWND();
}


void Editor::Update() const
{
    m_app.processEvents();
}
} // namespace blainn