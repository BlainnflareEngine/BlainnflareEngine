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
    void PostInit();
    void Destroy();

    void Show() const;
    void Update() const;

    HWND GetViewportHWND();

    editor::inspector_widget &GetInspector() const;

    void SetContentDirectory(const Path &path);

    std::shared_ptr<editor::EditorSink<std::mutex>> GetEditorSink();

    YAML::Node GetEditorConfig();
    Path GetEditorConfigPath();

private:
    QApplication *m_app = nullptr;
    editor::editor_main *m_editorMain = nullptr;
    Path m_editorConfigFolder;

    std::shared_ptr<editor::EditorSink<std::mutex>> m_editorSink;

    Editor() = default;
    Editor(const Editor &) = delete;
    Editor &operator=(const Editor &) = delete;
    Editor(const Editor &&) = delete;
    Editor &operator=(const Editor &&) = delete;

    void CreateDefaultEditorConfig();
};

} // namespace Blainn
