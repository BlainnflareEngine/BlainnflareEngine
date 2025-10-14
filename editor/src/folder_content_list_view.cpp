//
// Created by gorev on 24.09.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_folder_content_list_view.h" resolved

#include "folder_content_list_view.h"

#include "../include/dialog/import_asset_dialog.h"
#include "FileSystemUtils.h"
#include "ui_folder_content_list_view.h"

#include <QFileSystemModel>
#include <QMimeData>
#include <qdir.h>
#include <qevent.h>


class QFileSystemModel;
namespace editor
{
folder_content_list_view::folder_content_list_view(QWidget *parent)
    : QListView(parent)
    , ui(new Ui::folder_content_list_view)
{
    ui->setupUi(this);

    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
}


folder_content_list_view::~folder_content_list_view()
{
    delete ui;
}


void folder_content_list_view::dropEvent(QDropEvent *event)
{
    const QMimeData *mime = event->mimeData();

    if (!mime->hasUrls()) return;


    QModelIndex targetIndex = indexAt(event->position().toPoint());
    QFileSystemModel *fsModel = qobject_cast<QFileSystemModel *>(model());

    if (!fsModel)
    {
        QListView::dropEvent(event);
        return;
    }

    QString targetPath;
    if (targetIndex.isValid())
    {
        QFileInfo fi = fsModel->fileInfo(targetIndex);
        if (fi.isDir())
        {
            targetPath = fi.absoluteFilePath();
        }
        else
        {
            targetPath = fi.absolutePath();
        }
    }
    else
    {
        targetPath = fsModel->rootPath();
    }

    for (const QUrl &url : mime->urls())
    {
        if (url.isEmpty()) continue;

        QString srcPath = url.toLocalFile();

        // TODO: get content folder path from editor config file
        if (!WasInFolderBefore(srcPath, QDir::currentPath().append("/Content")))
        {
            ImportAssetInfo info;
            info.originalPath = srcPath;
            info.destinationPath = targetPath + QDir::separator() + url.fileName();
            import_asset_dialog *dialog = GetImportAssetDialog(info);
            dialog->exec();

            if (dialog->result() == QDialog::Accepted)
                QFile::copy(info.originalPath, info.destinationPath);

        }
        else
        {
            MoveRecursively(targetPath, srcPath);
        }
    }

    event->acceptProposedAction();
}


void folder_content_list_view::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else
    {
        QListView::dragEnterEvent(event);
    }
}


void folder_content_list_view::dragMoveEvent(QDragMoveEvent *event)
{
    QModelIndex index = indexAt(event->position().toPoint());
    QFileSystemModel *fsModel = qobject_cast<QFileSystemModel *>(model());

    if (!fsModel)
    {
        QListView::dragMoveEvent(event);
        return;
    }

    bool canDrop = false;

    if (event->mimeData()->hasUrls())
    {
        if (index.isValid())
        {
            QFileInfo fi = fsModel->fileInfo(index);
            if (fi.isDir())
            {
                canDrop = true;
            }
        }
        else
        {
            canDrop = true;
        }
    }

    if (canDrop)
    {
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}
} // namespace editor
