//
// Created by gorev on 23.09.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_folders_widget.h" resolved

#include "folders_widget.h"
#include "ui_folders_widget.h"

#include <QFileSystemModel>
#include <QTreeView>
#include <qboxlayout.h>


namespace editor
{
folders_widget::folders_widget(QWidget *parent)
    : QWidget(parent)
    , m_ui(new Ui::folders_widget)
{
    m_ui->setupUi(this);

    m_fileSystemModel = eastl::make_unique<QFileSystemModel>(this);
    m_fileSystemModel->setFilter(QDir::NoDotAndDotDot | QDir::AllDirs);

    m_treeView = eastl::make_unique<QTreeView>(this);
    m_treeView->setModel(m_fileSystemModel.get());
    m_treeView->setHeaderHidden(true);
    m_treeView->hideColumn(1);
    m_treeView->hideColumn(2);
    m_treeView->hideColumn(3);

    m_layout = eastl::make_unique<QVBoxLayout>(this);
    m_layout->setSpacing(5);
    m_layout->setContentsMargins(0, 0, 0, 0);
    m_layout->addWidget(m_treeView.get());

    connect(m_treeView->selectionModel(), &QItemSelectionModel::currentChanged, this,
            &folders_widget::onSelectionChanged);
}


folders_widget::~folders_widget()
{
    m_ui.release();
    m_fileSystemModel.release();
    m_treeView.release();
    m_layout.release();
}


void folders_widget::SetContentDirectory(const QString &contentDirectory)
{
    m_treeView->setRootIndex(m_fileSystemModel->setRootPath(contentDirectory));
    onSelectionChanged(m_fileSystemModel->index(contentDirectory));
}


void folders_widget::onSelectionChanged(const QModelIndex &newSelection)
{
    if (newSelection.isValid())
    {
        QString path = m_fileSystemModel->filePath(newSelection);
        emit folderSelected(path);
    }
}

} // namespace editor
