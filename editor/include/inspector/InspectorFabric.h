//
// Created by gorev on 14.10.2025.
//

#pragma once

#include <qstring.h>

namespace editor
{
class material_inspector_content;
class mesh_inspector_content;
class inspector_content_base;
} // namespace editor
namespace editor
{

class InspectorFabric
{
public:
    inspector_content_base *GetInspector(const QString &file);

private:
    material_inspector_content *GetMaterialInspector(const QString &file);
    mesh_inspector_content *GetMeshInspector(const QString &file);
};

} // namespace editor