//
// Created by gorev on 21.09.2025.
//

#include "editor_main.h"

#include "Editor.h"
#include "EditorSink.h"
#include "Engine.h"
#include "FileSystemUtils.h"
#include "SettingsData.h"
#include "editor_settings.h"
#include "folder_content_widget.h"
#include "ui_editor_main.h"
#include "Navigation/NavigationSubsystem.h"
#include "components/NavMeshVolumeComponent.h"
#include "context-menu/ViewportSettingsContext.h"
#include "oclero/qlementine/style/QlementineStyle.hpp"

#include <QDesktopServices>
#include <QListView>
#include <fstream>

extern bool g_IsRunning;

namespace editor
{
editor_main::editor_main(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::editor_main)
{
    ui->setupUi(this);

    auto logger = std::make_shared<EditorSink<std::mutex>>(ui->consoleMessages);

    ui->folderContent->AddAdditionalView(ui->folders->GetTreeView());
    ui->folders->AddAdditionalView(ui->folderContent->GetListView());

    ui->m_stopButton->setCheckable(true);

    m_inspector = new inspector_widget();
    ui->inspectorTab->layout()->addWidget(m_inspector);

    connect(ui->folders, &folders_widget::folderSelected, ui->folderContent,
            &folder_content_widget::OnFolderSelectedPath);
    connect(ui->folders, &folders_widget::folderSelected, ui->pathBar, &path_bar_widget::SetCurrentPath);


    connect(ui->pathBar, &path_bar_widget::PathClicked, ui->folderContent,
            &folder_content_widget::OnFolderSelectedPath);
    connect(ui->pathBar, &path_bar_widget::PathClicked, ui->folders, &folders_widget::OnFolderSelectedPath);

    connect(ui->folderContent, &folder_content_widget::FolderSelected, ui->folders,
            &folders_widget::OnFolderSelectedPath);
    connect(ui->folderContent, &folder_content_widget::FolderSelected, ui->pathBar, &path_bar_widget::SetCurrentPath);


    connect(ui->Entities->selectionModel(), &QItemSelectionModel::selectionChanged, ui->folderContent->GetListView(),
            &QListView::clearSelection);
    connect(ui->folderContent->GetListView()->selectionModel(), &QItemSelectionModel::selectionChanged, ui->Entities,
            &QTreeView::clearSelection);


    connect(ui->ClearConsoleButton, &QPushButton::clicked, ui->consoleMessages, &console_messages_widget::ClearConsole);

    connect(ui->AddToScene, &QPushButton::clicked, this, &editor_main::OpenAddToScene);


    connect(ui->m_playButton, &QPushButton::clicked, this, &editor_main::OnStartPlayMode);
    connect(ui->m_stopButton, &QPushButton::toggled, this, &editor_main::OnStopPlayModeToggle);

    // Action bar
    ui->actionSave->setShortcut(Qt::CTRL + Qt::Key_S);
    ui->actionSave->setIcon(QIcon(":/icons/save.png"));
    ui->actionEditor_settings->setShortcut(Qt::CTRL + Qt::Key_P);
    ui->actionEditor_settings->setIcon(QIcon(":/icons/settings.png"));

    connect(ui->actionEditor_settings, &QAction::triggered, this, &editor_main::OnOpenSettings);
    connect(ui->actionSave, &QAction::triggered, this, &editor_main::OnSaveScene);
    connect(ui->actionBuildNavmesh, &QAction::triggered, this, &editor_main::OnBuildNavMesh);
    connect(ui->actionBuildGame, &QAction::triggered, this, &editor_main::StartGameBuild);

    auto docAction = ui->menuHelp->addAction("Documentation");
    auto supportAction = ui->menuHelp->addAction("Support");

    connect(docAction, &QAction::triggered,
            []() { QDesktopServices::openUrl(QUrl("https://github.com/BlainnflareEngine/BlainnflareEngine/wiki")); });

    // rickroll just for fun
    connect(supportAction, &QAction::triggered,
            []() { QDesktopServices::openUrl(QUrl("https://youtu.be/xvFZjo5PgG0?list=RDxvFZjo5PgG0")); });


    m_sceneEvents.emplace_back(
        Blainn::Scene::AddEventListener(Blainn::SceneEventType::SceneChanged,
                                        [this](const Blainn::SceneEventPointer &event)
                                        {
                                            auto sceneEvent = static_cast<Blainn::SceneChangedEvent *>(event.get());
                                            this->ui->m_sceneName->setText(sceneEvent->GetName().c_str());
                                        }),
        Blainn::SceneEventType::SceneChanged);

    m_viewportSettingsContext = new ViewportSettingsContext(ui->m_viewportSettings, this);
}


editor_main::~editor_main()
{
    for (auto &[event, type] : m_sceneEvents)
    {
        Blainn::Scene::RemoveEventListener(type, event);
    }

    delete ui;
}


void editor_main::PostInit()
{
    m_viewportSettingsContext->Initialize();
}


HWND editor_main::GetViewportHWND() const
{
    return reinterpret_cast<HWND>(ui->viewportTab->winId());
}


void editor_main::SetContentDirectory(const QString &path)
{
    m_contentPath = path;

    ui->folders->SetContentDirectory(m_contentPath);
    ui->pathBar->SetRootPath(m_contentPath);
}


void editor_main::closeEvent(QCloseEvent *event)
{
    // TODO: serialize something before exit
    QMainWindow::closeEvent(event);
    QCoreApplication::quit();
    g_IsRunning = false;
}


inspector_widget &editor_main::GetInspectorWidget()
{
    return *m_inspector;
}


console_messages_widget *editor_main::GetConsoleWidget() const
{
    return ui->consoleMessages;
}


void editor_main::paintEvent(QPaintEvent *event)
{
    BLAINN_PROFILE_FUNC();
    QMainWindow::paintEvent(event);
}

void editor_main::resizeEvent(QResizeEvent *event)
{
    BLAINN_PROFILE_FUNC();
    QMainWindow::resizeEvent(event);
}


void editor_main::OpenAddToScene() const
{
    QPoint pos = ui->AddToScene->rect().bottomLeft();
    ui->Entities->OpenContextMenu(ui->AddToScene->mapToGlobal(pos));
}


void editor_main::OnOpenSettings()
{
    SettingsData data = SettingsData(Blainn::Engine::GetContentDirectory());
    editor_settings *settings = new editor_settings(data, this);
    settings->show();
}


void editor_main::OnSaveScene()
{
    Blainn::Engine::GetActiveScene()->SaveScene();
    ui->Entities->SaveCurrentMeta();
}


void editor_main::OnBuildNavMesh()
{
    auto scene = Blainn::Engine::GetActiveScene();
    if (!scene) return;

    for (const auto &[entity, volume] : scene->GetAllEntitiesWith<Blainn::NavmeshVolumeComponent>().each())
    {
        Blainn::Path relativePath =
            Blainn::Path(scene->GetName().c_str()).replace_extension("") / (scene->GetName() + ".navmesh").c_str();

        Blainn::NavigationSubsystem::BakeNavMesh(*scene, Blainn::Entity(entity, scene.get()), relativePath);
        Blainn::NavigationSubsystem::LoadNavMesh(relativePath);
    }
}


void editor_main::OnStartPlayMode()
{
    if (!Blainn::Engine::IsPlayMode())
    {
        Blainn::Engine::StartPlayMode();
        ui->m_playButton->setIcon(QIcon(":/icons/stop.png"));
    }
    else
    {
        Blainn::Engine::EscapePlayMode();
        ui->m_playButton->setIcon(QIcon(":/icons/play.png"));
    }
}

void editor_main::OnStopPlayModeToggle()
{
    Blainn::Engine::TogglePausePlayMode();
}


void editor_main::StartGameBuild()
{
    if (m_buildProcess && m_buildProcess->state() == QProcess::Running)
    {
        BF_WARN("Build already in progress");
        return;
    }

    QString appDir = QApplication::applicationDirPath();
    QString scriptPath = appDir + "/build/build.bat";

    if (!QFile::exists(scriptPath))
    {
        BF_ERROR("Build script not found: {}", scriptPath.toStdString());
        return;
    }

    m_buildProcess = new QProcess(this);
    m_buildProcess->setWorkingDirectory(QDir::currentPath());

    connect(m_buildProcess, &QProcess::finished, this, &editor_main::OnBuildFinished);
    connect(m_buildProcess, qOverload<QProcess::ProcessError>(&QProcess::errorOccurred), this,
            [](QProcess::ProcessError error) { BF_ERROR("Build process error: {}", static_cast<int>(error)); });

    connect(m_buildProcess, &QProcess::readyReadStandardOutput, this, &editor_main::OnBuildOutputAvailable);
    connect(m_buildProcess, &QProcess::readyReadStandardError, this, &editor_main::OnBuildErrorAvailable);

    m_buildProcess->start("cmd", QStringList() << "/c" << scriptPath);

    BF_DEBUG("Build started: {}", ToString(scriptPath));
}


void editor_main::OnBuildFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    if (exitCode == 0 && exitStatus == QProcess::NormalExit)
    {
        BF_DEBUG("Build succeeded!");
    }
    else
    {
        BF_ERROR("Build failed with code: {}", exitCode);
    }

    m_buildProcess->deleteLater();
    m_buildProcess = nullptr;
}


void editor_main::OnBuildOutputAvailable()
{
    QByteArray output = m_buildProcess->readAllStandardOutput();
    BF_DEBUG("BUILD OUT: {}", ToString(output));
}


void editor_main::OnBuildErrorAvailable()
{
    QByteArray error = m_buildProcess->readAllStandardError();
    BF_ERROR("BUILD ERR: {}", ToString(error));
}

} // namespace editor
