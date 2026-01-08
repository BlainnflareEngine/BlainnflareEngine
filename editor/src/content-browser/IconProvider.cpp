//
// Created by gorev on 08.10.2025.
//

#include "IconProvider.h"

#include "FileSystemUtils.h"

namespace editor
{

QIcon IconProvider::icon(IconType icon) const
{
    switch (icon)
    {
    case Folder:
        return QIcon(":/icons/folder.png");
    case File:
        return QIcon(":/icons/file.png");
    default:
        return QFileIconProvider::icon(icon);
    }
}


QIcon IconProvider::icon(const QFileInfo &file_info) const
{
    if (file_info.isDir()) return QIcon(":/icons/folder.png");

    QString suffix = file_info.suffix().toLower();
    QString fileName = file_info.fileName();

    if (formats::supportedTextureFormats.contains(suffix)) return QIcon(":/icons/image.png");

    if (fileName.endsWith(".AI.lua", Qt::CaseInsensitive)) return QIcon(":/icons/behaviorScript.png");

    if (suffix == formats::scriptFormat) return QIcon(":/icons/lua.png");

    if (suffix == formats::materialFormat) return QIcon(":/icons/material.png");

    if (formats::supported3DFormats.contains(suffix)) return QIcon(":/icons/model.png");

    if (suffix == formats::sceneFormat) return QIcon(":/icons/scene.png");

    return QIcon(":/icons/file.png");
}
} // namespace editor