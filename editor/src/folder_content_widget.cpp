//
// Created by gorev on 23.09.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_folder_content_widget.h" resolved

#include "folder_content_widget.h"
#include "ui_folder_content_widget.h"

#include <QFileSystemModel>
#include <QListView>
#include <iostream>
#include <qlayout.h>

namespace editor
{
folder_content_widget::folder_content_widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::folder_content_widget)
{
    ui->setupUi(this);

    m_fileSystemModel = eastl::make_unique<QFileSystemModel>(this);
    m_fileSystemModel->setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);

    m_listView = eastl::make_unique<QListView>(this);
    m_listView->setModel(m_fileSystemModel.get());
    m_listView->setViewMode(QListView::IconMode);
    m_listView->setResizeMode(QListView::Adjust);
    m_listView->setGridSize(QSize(100, 100));

    m_layout = eastl::make_unique<QVBoxLayout>(this);
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->setSpacing(5);
    layout()->addWidget(m_listView.get());

    connect(m_listView.get(), &QListView::doubleClicked, this, &folder_content_widget::onFolderSelectedIndex);
}


folder_content_widget::~folder_content_widget()
{
    m_fileSystemModel.reset();
    m_listView.reset();
    m_layout.reset();
    ui.release();
}


void folder_content_widget::SetContentDirectory(const QString &contentDirectory) const
{
    m_listView->setRootIndex(m_fileSystemModel->setRootPath(contentDirectory));
}

void folder_content_widget::onFolderSelectedPath(const QString &newPath)
{
    SetContentDirectory(newPath);
}


void folder_content_widget::onFolderSelectedIndex(const QModelIndex &newPath)
{
    if (m_fileSystemModel->isDir(newPath))
    {
        QString path = m_fileSystemModel->filePath(newPath);
        emit folderSelected(path);
        SetContentDirectory(path);
    }
}

} // namespace editor
