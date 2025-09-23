//
// Created by gorev on 21.09.2025.
//

#pragma once
#include "editor_main.h"


#include <QApplication>

namespace Blainn
{

class Editor
{
public:
    Editor(int &argc, char **argv);
    void Show();
    HWND GetViewportHWND();
    void Update() const;

private:
    QApplication m_app;
    editor::editor_main m_editor_main;
};

} // namespace Blainn
