//
// Created by gorev on 21.09.2025.
//

#include "Editor.h"
#include "ui_editor_main.h"

#include <fstream>
#include <yaml-cpp/yaml.h>
#include <QApplication>

namespace Blainn
{
Editor &Editor::GetInstance()
{
    static Editor instance;

    return instance;
}


void Editor::Init(int argc, char **argv)
{
    using namespace std::filesystem;
    m_app = new QApplication(argc, argv);

    qRegisterMetaType<editor::LogMessage>("LogMessage");

    QApplication::setStyle("fusion");
    QFont editorFont("Century Gothic", 12);
    QApplication::setFont(editorFont);

    m_editorMain = new editor::editor_main();

    BF_DEBUG("Current working directory - " + current_path().string());
    m_editorConfigFolder = current_path() / "Config";

    if (!exists(m_editorConfigFolder))
    {
        CreateDefaultEditorConfig();
    }
    else
    {
        YAML::Node config = YAML::LoadFile((m_editorConfigFolder / "EditorConfig.yaml").string());
        m_contentDirectory = config["ContentDirectory"].as<std::string>();
    }

    BF_DEBUG("Content directory - " + m_contentDirectory.string());
    m_editorMain->SetContentDirectory(QString::fromStdString(m_contentDirectory.string()));

    Log::AddSink(GetEditorSink());
}


void Editor::Destroy()
{
    m_app->quit();
    delete m_editorMain;
    delete m_app;
}


void Editor::Show() const
{
    BF_INFO("Editor::Show()");
    m_editorMain->showMaximized();
}


HWND Editor::GetViewportHWND()
{
    return m_editorMain->GetViewportHWND();
}


void Editor::Update() const
{
    m_app->processEvents();
}


Path &Editor::GetContentDirectory()
{
    return m_contentDirectory;
}


void Editor::SetContentDirectory(const std::filesystem::path &path)
{
    YAML::Node config = YAML::LoadFile((m_editorConfigFolder / "EditorConfig.yaml").string());
    config["ContentDirectory"] = path.string();
    m_contentDirectory = path;
    m_editorMain->SetContentDirectory(QString::fromStdString(m_contentDirectory.string()));
}


std::shared_ptr<editor::EditorSink<std::mutex>> Editor::GetEditorSink() const
{
    return std::make_shared<editor::EditorSink<std::mutex>>(m_editorMain->GetConsoleWidget());
}


void Editor::CreateDefaultEditorConfig()
{
    using namespace std::filesystem;

    create_directory(m_editorConfigFolder);

    YAML::Node config;
    m_contentDirectory = current_path() / "Content";
    create_directory(m_contentDirectory);

    config["ContentDirectory"] = m_contentDirectory.string();
    const path configFilePath = m_editorConfigFolder / "EditorConfig.yaml";
    std::ofstream fout(configFilePath.string());
    fout << config;
}

} // namespace Blainn