//
// Created by gorev on 21.09.2025.
//

#pragma once
#include "editor_main.h"


#include <QApplication>
#include <QPointer>

namespace Blainn
{

class Editor
{
public:
    static Editor& GetInstance();

    void Init(int &argc, char **argv);
    void Destroy();

    void Show();
    HWND GetViewportHWND();
    void Update() const;

    std::filesystem::path GetContentDirectory() const;
    void SetContentDirectory(const std::filesystem::path &path);

private:
    QApplication* m_app;
    editor::editor_main* m_editorMain;
    std::filesystem::path m_editorConfigFolder;
    std::filesystem::path m_contentDirectory;

    Editor() = default;
    Editor(const Editor &) = delete;
    Editor &operator=(const Editor &) = delete;
    Editor(const Editor &&) = delete;
    Editor &operator=(const Editor &&) = delete;

    void CreateDefaultEditorConfig();
};

} // namespace Blainn
