//
// Created by gorev on 24.09.2025.
//

#pragma once
#include "EASTL/vector.h"


#include <QTreeView>
#include <qstring.h>


class QLabel;
namespace editor
{
class import_asset_dialog;
namespace Ui
{
class import_asset_dialog;
}

struct ImportAssetInfo
{
    QString originalPath;
    QString destinationPath;
};

inline static const QStringList supported3DFormats = {
    "obj",
    "gltf",
    "glb",
    "fbx",
};

namespace Filters
{
inline static QString ShaderFilter = "Shaders (*.cso);;All Files (*)";
inline static QString TextureFilter = "Textures (*.png *.dds *.jpg *.jpeg *.tga *.bmp);;All Files (*)";
} // namespace Filters


inline static const QStringList supportedTextureFormats = {"png", "jpg", "dds", "jpeg", "tga", "bmp"};

void OpenFileExternal(const QString &filePath);

void OpenFileExplorer(const QString &filePath);

void OpenFolderExplorer(const QString &path);

void OpenFolder(const QString &path, QAbstractItemView &itemView);

void OpenFolder(const QString &path, const eastl::vector<QAbstractItemView *> &itemViews);

// Why B? Simply because DeleteFile is WinAPI macro :)
void BDeleteFile(const QString &filePath);

void DeleteFolder(const QString &path);

// Performs full copy of source folder to destination folder and deletes source folder
bool CopyRecursively(const QString &sourceFolder, const QString &destFolder);

// Performs renaming of all files in target path
bool MoveRecursively(const QString &targetPath, const QString &srcPath);

bool WasInFolderBefore(const QString &filePath, const QString &contentFolderPath);

import_asset_dialog *GetImportAssetDialog(const ImportAssetInfo &info);

std::string ToString(const QString &str);

void SelectFile(QLabel &label, const QString &filter, const QString &relativeDir = QString());

void SetValueYAML(const std::string &path, const std::string &name, const std::string &value);

} // namespace editor