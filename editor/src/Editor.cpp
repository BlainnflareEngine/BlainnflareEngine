//
// Created by gorev on 21.09.2025.
//

#include "Editor.h"

#include "AssetManager.h"
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

    QList<QByteArray> formats = QImageReader::supportedImageFormats();
    qDebug() << "Supported formats:" << formats;

    qRegisterMetaType<editor::LogMessage>("LogMessage");

    auto *style = new oclero::qlementine::QlementineStyle(m_app);
    style->setThemeJsonPath(":/themes/dark.json");
    QApplication::setStyle(style);

    m_editorMain = new editor::editor_main();

    BF_DEBUG("Current working directory - " + current_path().string());
    m_editorConfigFolder = current_path() / "Config";

    if (!exists(m_editorConfigFolder))
    {
        CreateDefaultEditorConfig();
    }

    YAML::Node config = YAML::LoadFile((m_editorConfigFolder / "EditorConfig.yaml").string());
    Engine::SetContentDirectory(config["ContentDirectory"].as<std::string>());

    BF_DEBUG("Content directory - " + Engine::GetContentDirectory().string());
    m_editorMain->SetContentDirectory(QString::fromStdString(Engine::GetContentDirectory().string()));

    Log::AddSink(GetEditorSink());

    std::string defaultScene = config["DefaultScenePath"].as<std::string>();
    if (!AssetManager::SceneExists(defaultScene)) AssetManager::CreateScene(defaultScene);

    AssetManager::OpenScene(defaultScene);
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


std::shared_ptr<editor::EditorSink<std::mutex>> Editor::GetEditorSink() const
{
    return std::make_shared<editor::EditorSink<std::mutex>>(m_editorMain->GetConsoleWidget());
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

    AssetManager::GetInstance().OpenScene(Path(m_editorConfigFolder / ("Scene." + ToString(formats::sceneFormat))));
    const path configFilePath = m_editorConfigFolder / "EditorConfig.yaml";
    std::ofstream fout(configFilePath.string());
    fout << config;
}

} // namespace Blainn