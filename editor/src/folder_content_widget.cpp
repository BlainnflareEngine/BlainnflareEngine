//
// Created by gorev on 23.09.2025.
//


#include "folder_content_widget.h"

#include "ContentDelegate.h"
#include "ContentFilterProxyModel.h"
#include "Editor.h"
#include "FileSystemUtils.h"
#include "IconProvider.h"
#include "InspectorFabric.h"
#include "context-menu/ContentContextMenu.h"
#include "inspector_content_base.h"
#include "ui_folder_content_widget.h"

#include <QFileSystemModel>
#include <QListView>
#include <QMimeData>
#include <QSortFilterProxyModel>
#include <iostream>
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

    m_proxyModel = new ContentFilterProxyModel(this);
    m_proxyModel->setSourceModel(m_fileSystemModel);

    // TODO: move all this properties to folder_content_list_view
    m_listView = new folder_content_list_view(this);
    m_listView->setModel(m_proxyModel);
    m_listView->setViewMode(QListView::IconMode);
    m_listView->setContextMenuPolicy(Qt::CustomContextMenu);
    m_listView->setContentsMargins(10, 10, 10, 10);
    m_listView->setItemDelegate(new ContentDelegate(ContentDelegate::Elide, m_fileSystemModel, m_proxyModel, this));

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
    QModelIndex sourceIndex = m_fileSystemModel->setRootPath(contentDirectory);
    QModelIndex proxyIndex = m_proxyModel->mapFromSource(sourceIndex);
    m_listView->setRootIndex(proxyIndex);
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
    QModelIndex sourceIndex = m_proxyModel->mapToSource(index);

    if (m_fileSystemModel->isDir(sourceIndex))
    {
        QString path = m_fileSystemModel->filePath(sourceIndex);

        emit FolderSelected(path);
        SetContentDirectory(path);
    }
    else if (m_fileSystemModel->fileInfo(sourceIndex).isFile())
    {
        OpenFileExternal(m_fileSystemModel->fileInfo(sourceIndex).filePath());
    }
}


void folder_content_widget::OnFileSelectedPath(const QModelIndex &index)
{
    QModelIndex sourceIndex = m_proxyModel->mapToSource(index);

    InspectorFabric fabric;
    auto inspector = fabric.GetInspector(m_fileSystemModel->filePath(sourceIndex));
    Blainn::Editor::GetInstance().GetInspector().SetItem(inspector);
}

} // namespace editor
