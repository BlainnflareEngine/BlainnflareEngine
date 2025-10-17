//
// Created by gorev on 21.09.2025.
//

#pragma once
#include "EditorSink.h"
#include "aliases.h"
#include "editor_main.h"
#include "inspector_widget.h"


#include <QApplication>

namespace Blainn
{

class Editor
{
public:
    static Editor &GetInstance();

    void Init(int argc, char **argv);
    void Destroy();

    void Show() const;
    void Update() const;

    HWND GetViewportHWND();
    Path &GetContentDirectory();

    editor::inspector_widget &GetInspector() const;

    void SetContentDirectory(const Path &path);

    std::shared_ptr<editor::EditorSink<std::mutex>> GetEditorSink() const;

private:
    QApplication *m_app;
    editor::editor_main *m_editorMain;
    Path m_editorConfigFolder;
    Path m_contentDirectory;

    Editor() = default;
    Editor(const Editor &) = delete;
    Editor &operator=(const Editor &) = delete;
    Editor(const Editor &&) = delete;
    Editor &operator=(const Editor &&) = delete;

    void CreateDefaultEditorConfig();
};

} // namespace Blainn
