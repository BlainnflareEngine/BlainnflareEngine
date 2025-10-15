//
// Created by gorev on 23.09.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_folder_content_widget.h" resolved

#include "folder_content_widget.h"

#include "../include/context-menu/ContentContextMenu.h"
#include "Editor.h"
#include "FileSystemUtils.h"
#include "IconProvider.h"
#include "InspectorFabric.h"
#include "inspector_content_base.h"
#include "ui_folder_content_widget.h"

#include <QFileSystemModel>
#include <QListView>
#include <QMimeData>
#include <iostream>
#include <qevent.h>
#include <qlayout.h>

namespace editor
{
folder_content_widget::folder_content_widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::folder_content_widget)
{
    ui->setupUi(this);

    m_iconProvider = new IconProvider();
    m_fileSystemModel = new QFileSystemModel(this);
    m_fileSystemModel->setFilter(QDir::NoDotAndDotDot | QDir::AllEntries);
    m_fileSystemModel->setIconProvider(m_iconProvider);

    // TODO: move all this properties to folder_content_list_view
    m_listView = new folder_content_list_view(this);
    m_listView->setModel(m_fileSystemModel);
    m_listView->setViewMode(QListView::IconMode);
    m_listView->setResizeMode(QListView::Adjust);
    m_listView->setGridSize(QSize(100, 100));
    m_listView->setIconSize(QSize(55, 55));
    m_listView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_listView->setContentsMargins(10, 10, 10, 10);

    m_layout = new QVBoxLayout(this);
    layout()->setContentsMargins(0, 0, 0, 0);
    layout()->setSpacing(5);
    layout()->addWidget(m_listView);

    m_contentContextMenu = new ContentContextMenu(*m_listView);

    m_fileContextMenu = new FileContextMenu(*m_listView);

    connect(m_listView, &QListView::doubleClicked, this, &folder_content_widget::OnEntrySelectedIndex);
    connect(m_listView, &QListView::clicked, this, &folder_content_widget::OnFileSelectedPath);
    connect(m_listView, &QWidget::customContextMenuRequested, m_contentContextMenu, &ContentContextMenu::OnContextMenu);
    connect(m_listView, &QListView::customContextMenuRequested, m_fileContextMenu, &FileContextMenu::OnContextMenu);
}


folder_content_widget::~folder_content_widget()
{
    delete m_fileSystemModel;
    delete m_listView;
    delete m_layout;
    delete ui;
}


void folder_content_widget::SetContentDirectory(const QString &contentDirectory) const
{
    m_listView->setRootIndex(m_fileSystemModel->setRootPath(contentDirectory));
}


void folder_content_widget::AddAdditionalView(QAbstractItemView *view)
{
    m_fileContextMenu->AddAdditionalView(view);
}


void folder_content_widget::RemoveAdditionalView(QAbstractItemView *view)
{
    m_fileContextMenu->RemoveAdditionalView(view);
}


QListView *folder_content_widget::GetListView() const
{
    return m_listView;
}


void folder_content_widget::OnFolderSelectedPath(const QString &newPath)
{
    SetContentDirectory(newPath);
}


void folder_content_widget::OnEntrySelectedIndex(const QModelIndex &index)
{
    if (m_fileSystemModel->isDir(index))
    {
        QString path = m_fileSystemModel->filePath(index);

        emit FolderSelected(path);
        SetContentDirectory(path);
    }
    else if (m_fileSystemModel->fileInfo(index).isFile())
    {
        OpenFileExternal(m_fileSystemModel->fileInfo(index).filePath());
    }
}


void folder_content_widget::OnFileSelectedPath(const QModelIndex &index)
{
    InspectorFabric fabric;
    auto inspector = fabric.GetInspector(m_fileSystemModel->filePath(index));
    Blainn::Editor::GetInstance().GetInspector().SetItem(inspector);
}

} // namespace editor
