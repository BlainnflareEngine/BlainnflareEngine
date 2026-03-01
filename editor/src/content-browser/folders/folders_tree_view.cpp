//
// Created by gorev on 25.09.2025.
//


#include "folders_tree_view.h"

#include "Editor.h"
#include "Engine.h"
#include "FileSystemUtils.h"
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
    delete ui;
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
        if (url.isEmpty()) continue;
        
        QString srcPath = url.toLocalFile();
        QString contentDir = QString::fromStdString(Blainn::Engine::GetContentDirectory().string());

        if (!WasInFolderBefore({srcPath, contentDir}))
        {
            ImportAsset({srcPath, targetPath, url});
        }
        else
        {
            MoveRecursively(targetPath, srcPath);
        }
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
