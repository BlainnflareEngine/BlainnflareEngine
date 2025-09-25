//
// Created by gorev on 25.09.2025.
//


#include "folders_tree_view.h"
#include "ui_folders_tree_view.h"

#include <QFileSystemModel>
#include <QMessageBox>
#include <QMimeData>
#include <qdir.h>
#include <qevent.h>

namespace editor
{
folders_tree_view::folders_tree_view(QWidget *parent)
    : QTreeView(parent)
    , ui(new Ui::folders_tree_view)
{
    ui->setupUi(this);

    setDragEnabled(true);
    setAcceptDrops(true);
    setDropIndicatorShown(true);
}


folders_tree_view::~folders_tree_view()
{
    ui.release();
}


void folders_tree_view::dropEvent(QDropEvent *event)
{
    const QMimeData *mime = event->mimeData();

    if (!mime->hasUrls()) return;


    QModelIndex targetIndex = indexAt(event->position().toPoint());
    QFileSystemModel *fsModel = qobject_cast<QFileSystemModel *>(model());

    if (!fsModel)
    {
        QTreeView::dropEvent(event);
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
            QMessageBox::warning(nullptr, "Error", "Failed to move file!");
        }

        qDebug() << "TODO: notify engine that some files were moved! folder_content_list_view::dropEvent";
    }

    event->acceptProposedAction();
}


void folders_tree_view::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else
    {
        QTreeView::dragEnterEvent(event);
    }
}


void folders_tree_view::dragMoveEvent(QDragMoveEvent *event)
{
    QModelIndex index = indexAt(event->position().toPoint());
    QFileSystemModel *fsModel = qobject_cast<QFileSystemModel *>(model());

    if (!fsModel)
    {
        QTreeView::dragMoveEvent(event);
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
