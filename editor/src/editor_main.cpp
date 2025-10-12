//
// Created by gorev on 21.09.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_editor_main.h" resolved

#include "editor_main.h"

#include "Editor.h"
#include "EditorSink.h"
#include "SettingsData.h"
#include "editor_settings.h"
#include "ui_editor_main.h"

#include <QResource>
#include <QTreeView>
#include <iostream>

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

    connect(ui->folderContent, &folder_content_widget::folderSelected, ui->folders,
            &folders_widget::OnFolderSelectedPath);
    connect(ui->folderContent, &folder_content_widget::folderSelected, ui->pathBar, &path_bar_widget::SetCurrentPath);


    connect(ui->actionEditor_settings, &QAction::triggered, this, &editor_main::OnOpenSettings);

    connect(ui->ClearConsoleButton, &QPushButton::clicked, ui->consoleMessages, &console_messages_widget::ClearConsole);
}


editor_main::~editor_main()
{
    delete ui;
}


HWND editor_main::GetViewportHWND() const
{
    return reinterpret_cast<HWND>(ui->sceneTab->winId());
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


console_messages_widget *editor_main::GetConsoleWidget() const
{
    return ui->consoleMessages;
}


void editor_main::OnOpenSettings()
{
    SettingsData data = SettingsData(Blainn::Editor::GetInstance().GetContentDirectory());
    eastl::unique_ptr<editor_settings> settings = eastl::make_unique<editor_settings>(data, this);
    settings->show();
    settings->exec();
}

} // namespace editor
