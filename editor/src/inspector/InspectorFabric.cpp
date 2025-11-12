//
// Created by gorev on 14.10.2025.
//

#include "InspectorFabric.h"

#include "FileSystemUtils.h"
#include "material_inspector_content.h"
#include "mesh_inspector_content.h"

#include <qfileinfo.h>

namespace editor
{

inspector_content_base *InspectorFabric::GetInspector(const QString &file)
{
    QFileInfo fileInfo(file);
    if (!fileInfo.exists())
    {
        BF_ERROR("No such file or directory");
        return nullptr;
    }

    auto a = fileInfo.suffix().toLower();
    if (fileInfo.suffix().toLower() == "mat")
    {
        return GetMaterialInspector(file);
    }

    if (formats::supported3DFormats.contains(fileInfo.suffix().toLower()))
    {
        return GetMeshInspector(file);
    }

    BF_ERROR("This file extension is not supported yet");
    return nullptr;
}


material_inspector_content *InspectorFabric::GetMaterialInspector(const QString &file)
{
    auto inspector = new material_inspector_content(file);
    inspector->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    inspector->adjustSize();
    return inspector;
}


mesh_inspector_content *InspectorFabric::GetMeshInspector(const QString &file)
{
    auto inspector = new mesh_inspector_content(file);
    inspector->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    inspector->adjustSize();
    return inspector;
}


entity_inspector_content *InspectorFabric::GetEntityInspector(const EntityInspectorData &data)
{
    auto inspector = new entity_inspector_content(data);
    inspector->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    inspector->adjustSize();
    return inspector;
}
} // namespace editor