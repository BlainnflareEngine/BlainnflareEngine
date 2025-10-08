//
// Created by gorev on 21.09.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_editor_main.h" resolved

#include "editor_main.h"

#include "Editor.h"
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

    ui->folderContent->AddAdditionalView(ui->folders->GetTreeView());
    ui->folders->AddAdditionalView(ui->folderContent->GetListView());

    connect(ui->folders, &folders_widget::folderSelected, ui->folderContent,
            &folder_content_widget::OnFolderSelectedPath);

    connect(ui->folderContent, &folder_content_widget::folderSelected, ui->folders,
            &folders_widget::OnFolderSelectedPath);

    connect(ui->actionEditor_settings, &QAction::triggered, this, &editor_main::OnOpenSettings);
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
}


void editor_main::closeEvent(QCloseEvent *event)
{
    // TODO: serialize something before exit
    QMainWindow::closeEvent(event);
    QCoreApplication::quit();
}


void editor_main::OnOpenSettings()
{
    SettingsData data = SettingsData(Blainn::Editor::GetInstance().GetContentDirectory());
    eastl::unique_ptr<editor_settings> settings = eastl::make_unique<editor_settings>(data, this);
    settings->show();
    settings->exec();
}

} // namespace editor
