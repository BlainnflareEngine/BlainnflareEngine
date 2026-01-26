//
// Created by gorev on 21.09.2025.
//

#include "Editor.h"

#include "AssetManager.h"
#include "EditorRegistry.h"
#include "Engine.h"
#include "FileSystemUtils.h"
#include "ui_editor_main.h"

#include <QApplication>
#include <QImageReader>
#include <fstream>
#include <oclero/qlementine.hpp>
#include <yaml-cpp/yaml.h>

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

    editor::InitializeEditorWidgets();

    QList<QByteArray> formats = QImageReader::supportedImageFormats();
    qRegisterMetaType<editor::LogMessage>("LogMessage");

    auto *style = new oclero::qlementine::QlementineStyle(m_app);
    style->setThemeJsonPath(":/themes/dark.json");

    // TODO: add switching themes
    // style->setThemeJsonPath(":/themes/light.json");
    QApplication::setStyle(style);

    BF_DEBUG("Current working directory - " + current_path().string());
    m_editorConfigFolder = current_path() / "Config";

    if (!exists(m_editorConfigFolder) || !exists(m_editorConfigFolder / "EditorConfig.yaml"))
    {
        CreateDefaultEditorConfig();
    }

    YAML::Node config = YAML::LoadFile((m_editorConfigFolder / "EditorConfig.yaml").string());
    Engine::SetContentDirectory(config["ContentDirectory"].as<std::string>());

    BF_DEBUG("Content directory - " + Engine::GetContentDirectory().string());

    if (config["DebugLines"]) RenderSubsystem::GetInstance().SetEnableDebug(config["DebugLines"].as<bool>());
    if (config["VSync"]) RenderSubsystem::GetInstance().SetVSyncEnabled(config["VSync"].as<bool>());

    std::string defaultScene = config["DefaultScenePath"].as<std::string>();
    if (!AssetManager::SceneExists(defaultScene)) AssetManager::CreateScene(defaultScene);

    AssetManager::OpenScene(defaultScene);

    m_editorMain = new editor::editor_main();
    m_editorMain->setWindowIcon(QIcon(":/icons/logo.png"));
    m_editorMain->SetContentDirectory(editor::ToQString(Engine::GetContentDirectory().string().c_str()));

    Log::AddSink(GetEditorSink());
}


void Editor::PostInit()
{
    m_editorMain->PostInit();
}


void Editor::Destroy()
{
    m_app->quit();
    delete m_editorMain;
    delete m_app;
    Log::RemoveSink(m_editorSink);
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
    BLAINN_PROFILE_SCOPE(Editor loop);
    m_app->processEvents();
}


editor::inspector_widget &Editor::GetInspector() const
{
    return m_editorMain->GetInspectorWidget();
}


void Editor::SetContentDirectory(const Path &path)
{
    YAML::Node config = YAML::LoadFile((m_editorConfigFolder / "EditorConfig.yaml").string());
    config["ContentDirectory"] = path.string();
    Engine::SetContentDirectory(path);
    m_editorMain->SetContentDirectory(QString::fromStdString(path.string()));
}


std::shared_ptr<editor::EditorSink<std::mutex>> Editor::GetEditorSink()
{
    if (!m_editorSink)
        m_editorSink = std::make_shared<editor::EditorSink<std::mutex>>(m_editorMain->GetConsoleWidget());
    return m_editorSink;
}


YAML::Node Editor::GetEditorConfig()
{
    return YAML::LoadFile((m_editorConfigFolder / "EditorConfig.yaml").string());
}


void Editor::CreateDefaultEditorConfig()
{
    using namespace std::filesystem;
    using namespace editor;

    create_directory(m_editorConfigFolder);

    YAML::Node config;
    create_directory(current_path() / "Content");

    config["ContentDirectory"] = (current_path() / "Content").string();
    config["DefaultScenePath"] = "Scene." + ToString(formats::sceneFormat);
    config["DebugLines"] = true;
    config["VSync"] = true;

    if (AssetManager::GetInstance().SceneExists(config["DefaultScenePath"].as<std::string>()))
    {
        AssetManager::GetInstance().OpenScene(config["DefaultScenePath"].as<std::string>());
    }
    else
    {
        AssetManager::GetInstance().CreateScene(config["DefaultScenePath"].as<std::string>());
    }

    const path configFilePath = m_editorConfigFolder / "EditorConfig.yaml";
    std::ofstream fout(configFilePath.string());
    fout << config;
}


Path Editor::GetEditorConfigPath()
{
    return m_editorConfigFolder / "EditorConfig.yaml";
}

} // namespace Blainn