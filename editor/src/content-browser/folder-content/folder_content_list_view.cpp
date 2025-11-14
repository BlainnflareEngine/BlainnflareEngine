//
// Created by gorev on 24.09.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_folder_content_list_view.h" resolved

#include "folder_content_list_view.h"

#include "Editor.h"
#include "Engine.h"
#include "FileSystemUtils.h"
#include "import_asset_dialog.h"
#include "ui_folder_content_list_view.h"

#include <QFileSystemModel>
#include <QMimeData>
#include <QSortFilterProxyModel>
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

    QFileSystemModel *fsModel = nullptr;
    QModelIndex sourceTargetIndex = targetIndex;

    if (QAbstractProxyModel *proxyModel = qobject_cast<QAbstractProxyModel *>(model()))
    {
        fsModel = qobject_cast<QFileSystemModel *>(proxyModel->sourceModel());
        if (targetIndex.isValid())
        {
            sourceTargetIndex = proxyModel->mapToSource(targetIndex);
        }
    }
    else
    {
        fsModel = qobject_cast<QFileSystemModel *>(model());
    }

    if (!fsModel)
    {
        QListView::dropEvent(event);
        return;
    }

    QString targetPath;
    if (sourceTargetIndex.isValid())
    {
        QFileInfo fi = fsModel->fileInfo(sourceTargetIndex);
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

        if (!WasInFolderBefore(srcPath, contentDir))
        {
            ImportAsset(srcPath, targetPath, url);
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
    QModelIndex targetIndex = indexAt(event->position().toPoint());

    QFileSystemModel *fsModel = nullptr;
    QModelIndex sourceTargetIndex = targetIndex;

    if (QAbstractProxyModel *proxyModel = qobject_cast<QAbstractProxyModel *>(model()))
    {
        fsModel = qobject_cast<QFileSystemModel *>(proxyModel->sourceModel());
        if (targetIndex.isValid())
        {
            sourceTargetIndex = proxyModel->mapToSource(targetIndex);
        }
    }
    else
    {
        fsModel = qobject_cast<QFileSystemModel *>(model());
    }

    if (!fsModel)
    {
        QListView::dragMoveEvent(event);
        return;
    }


    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
        return;
    }

    event->ignore();
}
} // namespace editor
