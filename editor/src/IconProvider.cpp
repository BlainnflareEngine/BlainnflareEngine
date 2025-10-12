//
// Created by gorev on 08.10.2025.
//

#include "../include/IconProvider.h"

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

    if (supportedTextureFormats.contains(file_info.suffix().toLower())) return QIcon(":/icons/image.png");

    if (file_info.suffix().toLower() == "lua") return QIcon(":/icons/lua.png");

    if (file_info.suffix().toLower() == "mat") return QIcon(":/icons/material.png");

    if (file_info.suffix().toLower() == "blainn") return QIcon(":/icons/meta.png");

    if (supported3DFormats.contains(file_info.suffix().toLower())) return QIcon(":/icons/model.png");

    return QFileIconProvider::icon(file_info);
}
} // namespace editor