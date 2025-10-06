//
// Created by gorev on 02.10.2025.
//

#include "FileSystemUtils.h"
#include "../include/dialog/import_asset_dialog.h"
#include "../include/dialog/import_model_dialog.h"

#include <QDesktopServices>
#include <QFileSystemModel>
#include <QListView>
#include <QMessageBox>
#include <QMimeData>
#include <qdir.h>
#include <qevent.h>
#include <qfileinfo.h>


namespace editor
{
void OpenFileExternal(const QString &filePath)
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


void OpenFileExplorer(const QString &filePath)
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


void OpenFolderExplorer(const QString &path)
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


void OpenFolder(const QString &path, QAbstractItemView &itemView)
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


void OpenFolder(const QString &path, const eastl::vector<QAbstractItemView *> &itemViews)
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


void BDeleteFile(const QString &filePath)
{
    qDebug() << "TODO: should notify engine that script was deleted!";

    QFile file(filePath);
    if (!file.exists())
    {
        QMessageBox::warning(nullptr, "Error", "File not found!");
        return;
    }

    auto reply = QMessageBox::question(nullptr, "Delete file", "Are you sure?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) file.remove();
}


void DeleteFolder(const QString &path)
{
    qDebug() << "TODO: should notify engine that folder potentially had scripts! FileSystemUtils::DeleteFolder";
    // could be done with listContents(const QString& path)

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


bool CopyRecursively(const QString &sourceFolder, const QString &destFolder)
{
    bool success = false;
    QDir sourceDir(sourceFolder);

    if (!sourceDir.exists()) return false;

    QDir destDir(destFolder);
    if (!destDir.exists()) destDir.mkdir(destFolder);

    QStringList files = sourceDir.entryList(QDir::Files);
    for (int i = 0; i < files.count(); i++)
    {
        QString srcName = sourceFolder + QDir::separator() + files[i];
        QString destName = destFolder + QDir::separator() + files[i];
        success = QFile::copy(srcName, destName);
        qDebug() << "TODO: should notify engine that we created new file!";

        if (!success) return false;
    }

    files.clear();
    files = sourceDir.entryList(QDir::AllDirs | QDir::NoDotAndDotDot);
    for (int i = 0; i < files.count(); i++)
    {
        QString srcName = sourceFolder + QDir::separator() + files[i];
        QString destName = destFolder + QDir::separator() + files[i];
        qWarning() << "TODO: should notify engine that we created new file!" << files[i];

        success = CopyRecursively(srcName, destName);

        if (!success) return false;
    }

    return true;
}


bool MoveRecursively(const QString &targetPath, const QString &srcPath)
{
    QFileInfo srcFi(srcPath);
    QString destPath = targetPath + "/" + srcFi.fileName();

    if (srcPath == destPath)
    {
        return true;
    }

    if (!QFile::rename(srcPath, destPath))
    {
        QMessageBox::warning(nullptr, "Error", "Failed to move files!");
        return false;
    }

    // TODO: notify engine that some files were moved!
    return true;
}


bool WasInFolderBefore(const QString &filePath, const QString &contentFolderPath)
{
    QDir projectDir(contentFolderPath);
    QString absFilePath = QFileInfo(filePath).absoluteFilePath();

    QString canonProjectDir = projectDir.canonicalPath();
    QString canonFilePath = QFileInfo(absFilePath).canonicalFilePath();

    if (canonProjectDir.isEmpty() || canonFilePath.isEmpty()) return false;

    return canonFilePath.startsWith(canonProjectDir, Qt::CaseInsensitive);
}


import_asset_dialog *GetImportAssetDialog(const ImportAssetInfo &info)
{
    if (supported3DFormats.contains(QFileInfo(info.path).suffix().toLower()))
    {
        BF_INFO("Showing model dialog dialog.");
        return new import_model_dialog(info);
    }

    return new import_asset_dialog(info);
}
} // namespace editor