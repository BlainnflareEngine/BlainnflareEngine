//
// Created by gorev on 24.09.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_folder_content_list_view.h" resolved

#include "folder_content_list_view.h"

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
    ui.release();
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

    bool success = true;
    for (const QUrl &url : mime->urls())
    {
        if (!url.isLocalFile()) continue;

        QString srcPath = url.toLocalFile();
        QFileInfo srcFi(srcPath);
        QString destPath = targetPath + "/" + srcFi.fileName();

        if (srcPath == destPath)
        {
            continue;
        }

        if (!QFile::rename(srcPath, destPath))
        {
            if (srcFi.isDir())
            {
                if (!CopyRecursively(srcPath, destPath))
                {
                    success = false;
                    continue;
                }
                QDir(srcPath).removeRecursively();
            }
            else
            {
                if (!QFile::copy(srcPath, destPath))
                {
                    success = false;
                    continue;
                }
                QFile::remove(srcPath);
            }
        }
    }

    if (success) event->acceptProposedAction();
    else event->ignore();
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
