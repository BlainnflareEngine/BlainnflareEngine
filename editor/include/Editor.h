//
// Created by gorev on 21.09.2025.
//

#pragma once
#include "EditorSink.h"
#include "aliases.h"
#include "editor_main.h"


#include <QApplication>
#include <QPointer>

namespace Blainn
{

class Editor
{
public:
    static Editor &GetInstance();

    void Init(int argc, char **argv);
    void Destroy();

    void Show() const;
    HWND GetViewportHWND();
    void Update() const;

    Path &GetContentDirectory();
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
