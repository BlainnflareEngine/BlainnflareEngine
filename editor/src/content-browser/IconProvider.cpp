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

    if (supportedTextureFormats.contains(suffix)) return QIcon(":/icons/image.png");

    if (suffix == scriptFormat) return QIcon(":/icons/lua.png");

    if (suffix == materialFormat) return QIcon(":/icons/material.png");

    if (supported3DFormats.contains(suffix)) return QIcon(":/icons/model.png");

    if (suffix == sceneFormat) return QIcon(":/icons/scene.png");

    return QIcon(":/icons/file.png");
}
} // namespace editor