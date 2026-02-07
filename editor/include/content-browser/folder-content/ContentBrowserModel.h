//
// Created by gorev on 06.02.2026.
//

#pragma once
#include "FileSystemUtils.h"
#include "MimeFormats.h"


#include <QFileSystemModel>
#include <QMimeData>

namespace editor
{
class ContentBrowserModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit ContentBrowserModel(QObject *parent = nullptr)
        : QFileSystemModel(parent) {};

    QStringList mimeTypes() const override
    {
        QStringList types = QFileSystemModel::mimeTypes();
        types << MIME_PREFAB;
        return types;
    }

    QMimeData *mimeData(const QModelIndexList &indexes) const override
    {
        QMimeData *mimeData = QFileSystemModel::mimeData(indexes);
        bool hasPrefab = false;

        for (const auto &index : indexes)
        {
            QString filePath = this->filePath(index);
            QFileInfo fileInfo(filePath);

            if (fileInfo.suffix().compare(formats::prefabFormat, Qt::CaseInsensitive) == 0)
            {
                hasPrefab = true;
                break;
            }
        }

        if (hasPrefab)
        {
            QByteArray prefabData = eastl::move(EncodePrefabData(indexes, this));
            mimeData->setData(MIME_PREFAB, prefabData);
        }

        return mimeData;
    }
};
} // namespace editor