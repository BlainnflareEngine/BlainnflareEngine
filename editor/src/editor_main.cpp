//
// Created by gorev on 21.09.2025.
//


#include "editor_main.h"

#include "Editor.h"
#include "EditorSink.h"
#include "Engine.h"
#include "SettingsData.h"
#include "editor_settings.h"
#include "folder_content_widget.h"
#include "ui_editor_main.h"

#include <QListView>

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


    connect(ui->m_playButton,  &QPushButton::clicked, this, &editor_main::OnStartPlayMode);
    connect(ui->m_stopButton,  &QPushButton::clicked, this, &editor_main::OnEscapePlayMode);

    // Action bar
    ui->actionSave->setShortcut(Qt::CTRL + Qt::Key_S);
    ui->actionSave->setIcon(QIcon(":/icons/save.png"));
    ui->actionEditor_settings->setShortcut(Qt::CTRL + Qt::Key_P);
    ui->actionEditor_settings->setIcon(QIcon(":/icons/settings.png"));

    connect(ui->actionEditor_settings, &QAction::triggered, this, &editor_main::OnOpenSettings);
    connect(ui->actionSave, &QAction::triggered, this, &editor_main::OnSaveScene);
}


editor_main::~editor_main()
{
    delete ui;
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
}


inspector_widget &editor_main::GetInspectorWidget()
{
    return *ui->m_inspector;
}


console_messages_widget *editor_main::GetConsoleWidget() const
{
    return ui->consoleMessages;
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


void editor_main::OnStartPlayMode()
{
    Blainn::Engine::StartPlayMode();
}


void editor_main::OnEscapePlayMode()
{
    Blainn::Engine::EscapePlayMode();
}

} // namespace editor
