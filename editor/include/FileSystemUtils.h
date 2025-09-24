//
// Created by gorev on 24.09.2025.
//

#pragma once
#include "EASTL/vector.h"


#include <QDesktopServices>
#include <QFileSystemModel>
#include <QListView>
#include <QMessageBox>
#include <QTreeView>
#include <qdir.h>
#include <qfileinfo.h>
#include <qstring.h>
#include <qurl.h>

inline void OpenFileExternal(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists())
    {
        QMessageBox::warning(nullptr, "Error", "File not found!");
        return;
    }

    QUrl fileURL = QUrl::fromLocalFile(filePath);
    if (!QDesktopServices::openUrl(fileURL))
    {
        QMessageBox::warning(nullptr, "Error", "Failed to open file!");
    }
}


inline void OpenFileExplorer(const QString &filePath)
{
    QFileInfo fileInfo(filePath);
    if (!fileInfo.exists())
    {
        QMessageBox::warning(nullptr, "Error", "File not found!");
        return;
    }

    QUrl folderURL = QUrl::fromLocalFile(fileInfo.path());
    if (!QDesktopServices::openUrl(folderURL))
    {
        QMessageBox::warning(nullptr, "Error", "Failed to open file!");
    }
}


inline void OpenFolderExplorer(const QString &path)
{
    QDir dir(path);

    if (!dir.exists())
    {
        QMessageBox::warning(nullptr, "Error", "Directory not found!");
        return;
    }

    QUrl fileUrl = QUrl::fromLocalFile(path);
    if (!QDesktopServices::openUrl(fileUrl))
    {
        QMessageBox::warning(nullptr, "Error", "Failed to open folder!");
    }
}


inline void OpenFolder(const QString &path, QAbstractItemView &itemView)
{
    QDir dir(path);
    if (!dir.exists())
    {
        QMessageBox::warning(nullptr, "Error", "Directory not found!");
        return;
    }

    auto fileModel = qobject_cast<QFileSystemModel *>(itemView.model());
    QModelIndex index = fileModel->index(path);
    if (index.isValid())
    {
        fileModel->setRootPath(path);
        if (auto treeView = qobject_cast<QTreeView *>(&itemView))
        {
            treeView->setCurrentIndex(index);
            treeView->expand(index);
            treeView->scrollTo(index);
        }
        if (auto listView = qobject_cast<QListView *>(&itemView))
        {
            QModelIndex index = fileModel->index(path);
            listView->setRootIndex(index);
            listView->setCurrentIndex(index);
            listView->scrollTo(index);
        }
    }
}


inline void OpenFolder(const QString &path, const eastl::vector<QAbstractItemView *> &itemViews)
{
    QDir dir(path);
    if (!dir.exists())
    {
        QMessageBox::warning(nullptr, "Error", "Directory not found!");
        return;
    }

    for (const auto itemView : itemViews)
        OpenFolder(path, *itemView);
}


inline void DeleteFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.exists())
    {
        QMessageBox::warning(nullptr, "Error", "File not found!");
        return;
    }

    auto reply = QMessageBox::question(nullptr, "Delete file", "Are you sure?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) file.remove();
}


inline void DeleteFolder(const QString &path)
{
    QDir dir(path);

    if (!dir.exists())
    {
        QMessageBox::warning(nullptr, "Error", "Directory not found!");
        return;
    }

    auto reply = QMessageBox::question(nullptr, "Delete folder", "Are you sure?", QMessageBox::Yes | QMessageBox::No);
    if (reply != QMessageBox::Yes) return;

    if (!dir.removeRecursively()) QMessageBox::warning(nullptr, "Error", "Failed to recursively delete folder!");
}