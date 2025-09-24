//
// Created by gorev on 23.09.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_folder_content_widget.h" resolved

#include "folder_content_widget.h"

#include "../include/ContextMenu/ContentContextMenu.h"
#include "FileSystemUtils.h"
#include "ui_folder_content_widget.h"

#include <QFileSystemModel>
#include <QListView>
#include <iostream>
#include <qevent.h>
#include <qlayout.h>
#include <QMimeData>

namespace editor
{
folder_content_widget::folder_content_widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::folder_content_widget)
{
    ui->setupUi(this);

    m_fileSystemModel = eastl::make_unique<QFileSystemModel>(this);
    m_fileSystemModel->setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);

    // TODO: move all this properties to folder_content_list_view
    m_listView = eastl::make_unique<folder_content_list_view>(this);
    m_listView->setModel(m_fileSystemModel.get());
    m_listView->setViewMode(QListView::IconMode);
    m_listView->setResizeMode(QListView::Adjust);
    m_listView->setGridSize(QSize(100, 100));
    m_listView->setContextMenuPolicy(Qt::CustomContextMenu);

    m_layout = eastl::make_unique<QVBoxLayout>(this);
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->setSpacing(5);
    layout()->addWidget(m_listView.get());

    m_contentContextMenu = eastl::make_unique<ContentContextMenu>(*m_listView.get());

    eastl::vector<QAbstractItemView *> additionalViews;
    m_fileContextMenu = eastl::make_unique<FileContextMenu>(*m_listView.get());

    connect(m_listView.get(), &QListView::doubleClicked, this, &folder_content_widget::onEntrySelectedIndex);
    connect(m_listView.get(), &QWidget::customContextMenuRequested, m_contentContextMenu.get(),
            &ContentContextMenu::OnContextMenu);
    connect(m_listView.get(), &QListView::customContextMenuRequested, m_fileContextMenu.get(),
            &FileContextMenu::OnContextMenu);
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


void folder_content_widget::AddAdditionalView(QAbstractItemView *view)
{
    m_additionalViews.emplace_back(view);
    m_fileContextMenu->AddAdditionalView(view);
}


void folder_content_widget::RemoveAdditionalView(QAbstractItemView *view)
{
    m_additionalViews.erase_first(view);
    m_fileContextMenu->RemoveAdditionalView(view);
}


QListView *folder_content_widget::GetListView() const
{
    return m_listView.get();
}


void folder_content_widget::onFolderSelectedPath(const QString &newPath)
{
    SetContentDirectory(newPath);
}


void folder_content_widget::onEntrySelectedIndex(const QModelIndex &index)
{
    if (m_fileSystemModel->isDir(index))
    {
        QString path = m_fileSystemModel->filePath(index);
        emit folderSelected(path);
        SetContentDirectory(path);
    }
    else if (m_fileSystemModel->fileInfo(index).isFile())
    {
        OpenFileExternal(m_fileSystemModel->fileInfo(index).filePath());
    }
}

} // namespace editor
