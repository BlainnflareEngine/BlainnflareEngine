//
// Created by gorev on 24.09.2025.
//

// You may need to build the project (run Qt uic code generator) to get "ui_folder_content_list_view.h" resolved

#include "folder_content_list_view.h"

#include "Editor.h"
#include "Engine.h"
#include "FileSystemUtils.h"
#include "MimeFormats.h"
#include "Serializer.h"
#include "import_asset_dialog.h"
#include "ui_folder_content_list_view.h"
#include "components/PrefabComponent.h"
#include "scene/SceneManager.h"
#include "scene/SceneManagerTemplates.h"

#include <QFileSystemModel>
#include <QMimeData>
#include <QSortFilterProxyModel>
#include <qdir.h>
#include <qevent.h>
#include <QDrag>


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


void folder_content_list_view::HandleFileDrop(const QMimeData *mime, const QString &targetPath)
{
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
}


void folder_content_list_view::HandleEntityDrop(QDropEvent *event, const QString &targetPath)
{
    QByteArray encoded = event->mimeData()->data(MIME_ENTITY_UUID);

    if (encoded.isEmpty())
    {
        event->ignore();
        return;
    }

    QString uuidStr = QString::fromUtf8(encoded);
    Blainn::uuid uuid = Blainn::uuid::fromStrFactory(ToString(uuidStr));
    Blainn::Entity entity = Blainn::Engine::GetSceneManager().TryGetEntityWithUUID(uuid);

    QDir contentDir(Blainn::Engine::GetContentDirectory());

    QString prefabFilename = entity.Name().c_str() + QStringLiteral(".") + formats::prefabFormat;
    QString absolutePath = contentDir.absoluteFilePath(prefabFilename);
    QString relativePath = contentDir.relativeFilePath(absolutePath);

    entity.AddComponent<Blainn::PrefabComponent>().Path = ToString(relativePath);

    Blainn::Serializer::CreatePrefab(entity, ToString(relativePath));
    BF_DEBUG("Create prefab from entity {} Is valid {}", uuid.str(), entity.IsValid());
}


QString folder_content_list_view::GetTargetPath(const QModelIndex &index, QFileSystemModel *fsModel)
{
    if (index.isValid())
    {
        QFileInfo fi = fsModel->fileInfo(index);
        if (fi.isDir())
        {
            return fi.absoluteFilePath();
        }

        return fi.absolutePath();
    }

    return fsModel->rootPath();
}


void folder_content_list_view::dropEvent(QDropEvent *event)
{
    const QMimeData *mime = event->mimeData();

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

    QString targetPath = GetTargetPath(sourceTargetIndex, fsModel);

    if (mime->hasFormat(MIME_ENTITY_UUID))
    {
        HandleEntityDrop(event, targetPath);
        event->acceptProposedAction();
    }

    if (mime->hasUrls())
    {
        HandleFileDrop(mime, targetPath);
        event->acceptProposedAction();
    }
}


void folder_content_list_view::dragEnterEvent(QDragEnterEvent *event)
{
    qDebug() << event->mimeData()->formats();

    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else if (event->mimeData()->formats().contains(MIME_ENTITY_UUID))
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
    else if (event->mimeData()->formats().contains(MIME_ENTITY_UUID))
    {
        event->acceptProposedAction();
        return;
    }

    event->ignore();
}
} // namespace editor
