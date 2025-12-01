//
// Created by gorev on 02.10.2025.
//

#include "FileSystemUtils.h"

#include "../include/dialog/import_asset_dialog.h"
#include "../include/dialog/import_model_dialog.h"
#include "ContentFilterProxyModel.h"

#include <QDesktopServices>
#include <QFileDialog>
#include <QFileSystemModel>
#include <QLabel>
#include <QListView>
#include <QMessageBox>
#include <QMimeData>
#include <fstream>
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
        BF_ERROR("Directory not found!");
        return;
    }

    QAbstractItemModel *model = itemView.model();
    if (!model) return;

    if (auto proxyModel = qobject_cast<ContentFilterProxyModel *>(model))
    {
        OpenFolderWithProxy(path, itemView, *proxyModel);
    }
    else if (auto fileModel = qobject_cast<QFileSystemModel *>(model))
    {
        OpenFolderWithoutProxy(path, itemView, *fileModel);
    }
    else
    {
        QMessageBox::warning(nullptr, "Error", "Unsupported model type!");
    }
}


void OpenFolderWithProxy(const QString &path, QAbstractItemView &itemView, const ContentFilterProxyModel &proxyModel)
{
    QFileSystemModel *fileModel = qobject_cast<QFileSystemModel *>(proxyModel.sourceModel());
    if (!fileModel)
    {
        BF_ERROR("Proxy model source is not a file system model");
        return;
    }

    QModelIndex sourceIndex = fileModel->index(path);
    if (!sourceIndex.isValid()) return;

    fileModel->setRootPath(path);

    QModelIndex viewIndex = proxyModel.mapFromSource(sourceIndex);
    if (!viewIndex.isValid()) return;

    ApplyViewSettings(itemView, viewIndex, viewIndex);
}


void OpenFolderWithoutProxy(const QString &path, QAbstractItemView &itemView, QFileSystemModel &fileModel)
{
    QModelIndex sourceIndex = fileModel.index(path);
    if (!sourceIndex.isValid()) return;

    fileModel.setRootPath(path);

    ApplyViewSettings(itemView, sourceIndex, sourceIndex);
}


void ApplyViewSettings(QAbstractItemView &itemView, const QModelIndex &rootIndex, const QModelIndex &currentIndex)
{
    if (auto treeView = qobject_cast<QTreeView *>(&itemView))
    {
        treeView->setCurrentIndex(currentIndex);
        treeView->expand(currentIndex);
        treeView->scrollTo(currentIndex);
    }
    else if (auto listView = qobject_cast<QListView *>(&itemView))
    {
        listView->setRootIndex(rootIndex);
        listView->setCurrentIndex(currentIndex);
        listView->scrollTo(currentIndex);
    }
}


void OpenFolder(const QString &path, const eastl::vector<QAbstractItemView *> &itemViews)
{
    QDir dir(path);
    if (!dir.exists())
    {
        BF_ERROR("Directory not found!");
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
    QFileInfo srcFile(srcPath);
    QString destPath = targetPath + "/" + srcFile.fileName();

    if (srcPath == destPath)
    {
        return true;
    }


    if (!QFile::rename(srcPath, destPath))
    {
        BF_ERROR("Failed to move file {0} to {1}.", ToString(srcFile.fileName()), ToString(targetPath));
        return false;
    }

    if (formats::supported3DFormats.contains(srcFile.suffix()))
    {
        if (!QFile::rename(srcPath + "." + formats::metaFormat, destPath + "." + formats::metaFormat))
        {
            BF_ERROR("Failed to move file {0} to {1}.", ToString(srcFile.fileName()), ToString(targetPath));
            return false;
        }
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
    if (formats::supported3DFormats.contains(QFileInfo(info.originalPath).suffix().toLower()))
    {
        BF_INFO("Showing model dialog.");
        return new import_model_dialog(info);
    }

    return nullptr;
}


std::string ToString(const QString &str)
{
    return str.toUtf8().constData();
}

const char * ToCString(const QString &str)
{
    return str.toUtf8().constData();
}


eastl::string ToEASTLString(const QString &str)
{
    return str.toUtf8().constData();
}


QString ToQString(const eastl::string &str)
{
    return QString(str.c_str());
}


/**
 * This is depricated func, you should use SelectFileAsync instead
 */
void SelectFile(QLabel &label, const QString &filter, const QString &relativeDir)
{
    QString fileName = QFileDialog::getOpenFileName(
        nullptr, "Select Texture File", label.text().isEmpty() ? "." : QFileInfo(label.text()).absolutePath(), filter);

    if (!fileName.isEmpty() && relativeDir.isEmpty())
    {
        label.setText(fileName);
    }
    else if (!fileName.isEmpty())
    {
        QDir dir(relativeDir);
        label.setText(dir.relativeFilePath(fileName));
    }
}


void SelectFileAsync(QWidget *parent, const QString &title, const QString &initialDir, const QString &nameFilter,
                     std::function<void(const QString &selectedFile)> onAccepted)
{
    QFileDialog *dialog = new QFileDialog(parent);
    dialog->setWindowTitle(title);
    dialog->setFileMode(QFileDialog::ExistingFile);
    dialog->setNameFilter(nameFilter);
    dialog->setDirectory(initialDir);
    dialog->setOption(QFileDialog::DontUseNativeDialog, true);

    QObject::connect(dialog, &QFileDialog::finished, dialog,
                     [dialog, onAccepted](int result)
                     {
                         if (result == QDialog::Accepted && !dialog->selectedFiles().isEmpty())
                         {
                             QString filePath = dialog->selectedFiles().first();
                             if (onAccepted) onAccepted(filePath);
                         }
                         dialog->deleteLater();
                     });

    dialog->open();
}


void SetValueYAML(const std::string &path, const std::string &name, const std::string &value)
{
    YAML::Node node = YAML::LoadFile(path);

    if (!node) return;

    node[name] = value;
    std::ofstream fout(path);
    fout << node;
}


void ImportAsset(const QString &src, const QString &dest, const QUrl &url)
{
    ImportAssetInfo info;
    info.originalPath = src;
    info.destinationPath = dest + QDir::separator() + url.fileName();
    import_asset_dialog *dialog = GetImportAssetDialog(info);

    if (!dialog)
    {
        BF_INFO("Importing file without import dialog: {0}", ToString(info.originalPath));
        QFile::copy(info.originalPath, info.destinationPath);
        return;
    }

    dialog->exec();

    if (dialog->result() == QDialog::Accepted) QFile::copy(info.originalPath, info.destinationPath);
}
} // namespace editor