//
// Created by gorev on 05.02.2026.
//

#pragma once
#include <QFileSystemModel>
#include <QMimeData>
#include "Engine.h"

namespace editor
{
constexpr char MIME_ENTITY_UUID[] = "application/x-blainn-entity-uuid";
constexpr char MIME_PREFAB[] = "application/x-blainn-prefab";

struct PrefabDragInfo
{
    QString relativePath;
    QString fileName;
    QString absolutePath;
};

inline QByteArray EncodePrefabData(const QModelIndexList &indexes, const QFileSystemModel *model)
{
    QByteArray encodedData;
    QDataStream stream(&encodedData, QIODevice::WriteOnly);

    eastl::vector<PrefabDragInfo> prefabs;

    for (const auto &index : indexes)
    {
        if (!index.isValid() || index.column() != 0) continue;

        QString absolutePath = model->filePath(index);
        QFileInfo fileInfo(absolutePath);

        if (fileInfo.suffix().compare("prefab", Qt::CaseInsensitive) != 0) continue;

        QDir contentDir(Blainn::Engine::GetContentDirectory());
        QString relativePath = contentDir.relativeFilePath(absolutePath);

        prefabs.push_back({
            .relativePath = relativePath,
            .fileName = fileInfo.fileName(),
            .absolutePath = absolutePath,
        });
    }

    stream << static_cast<quint32>(prefabs.size());

    for (const auto &prefab : prefabs)
    {
        stream << prefab.relativePath;
        stream << prefab.fileName;
        stream << prefab.absolutePath;
    }

    return encodedData;
}

inline eastl::vector<PrefabDragInfo> DecodePrefabData(const QMimeData *mimeData)
{
    eastl::vector<PrefabDragInfo> prefabs;

    if (!mimeData->hasFormat(MIME_PREFAB)) return prefabs;

    QByteArray data = mimeData->data(MIME_PREFAB);
    QDataStream stream(&data, QIODevice::ReadOnly);

    quint32 count;
    stream >> count;

    prefabs.reserve(count);

    for (quint32 i = 0; i < count && !stream.atEnd(); ++i)
    {
        QString relativePath, fileName, absolutePath;
        stream >> relativePath >> fileName >> absolutePath;

        prefabs.push_back({.relativePath = relativePath, .fileName = fileName, .absolutePath = absolutePath});
    }

    return prefabs;
}
} // namespace editor
