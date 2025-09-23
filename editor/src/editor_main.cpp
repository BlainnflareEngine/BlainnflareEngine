//
// Created by gorev on 21.09.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_editor_main.h" resolved

#include "editor_main.h"
#include "ui_editor_main.h"

#include <iostream>

namespace editor
{
editor_main::editor_main(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::editor_main)
{
    ui->setupUi(this);

    connect(ui->folders, &folders_widget::folderSelected, ui->folderContent, &folder_content_widget::onFolderSelected);
}


editor_main::~editor_main()
{
    ui.release();
}


HWND editor_main::GetViewportHWND() const
{
    return reinterpret_cast<HWND>(ui->sceneTab->winId());
}
void editor_main::SetContentDirectory(const QString &path)
{
    m_contentPath = path;

    ui->folders->SetContentDirectory(m_contentPath);
    //ui->folderContent->SetContentDirectory(m_contentPath);
}

} // namespace editor
