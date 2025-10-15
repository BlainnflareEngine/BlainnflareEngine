//
// Created by gorev on 14.10.2025.
//

#include "InspectorFabric.h"

#include "material_inspector_content.h"

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
} // namespace editor