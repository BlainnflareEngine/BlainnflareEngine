//
// Created by gorev on 17.11.2025.
//

#include "entity/mesh_widget.h"

#include "Engine.h"
#include "FileSystemUtils.h"
#include "assimp/code/AssetLib/3MF/3MFXmlTags.h"
#include "components/MeshComponent.h"
#include "input-widgets/path_input_field.h"
#include "scene/EntityTemplates.h"
#include "spdlog/fmt/bundled/std.h"

#include <QLayout>

namespace editor
{
mesh_widget::mesh_widget(const Blainn::Entity &entity, QWidget *parent)
    : component_widget_base(entity, "Mesh", parent)
{
    m_path_input_field = new path_input_field("Mesh path", {}, this);
    m_path_input_field->SetExtensions(formats::supported3DFormats);
    layout()->addWidget(m_path_input_field);

    UpdatePath();

    connect(m_path_input_field, &path_input_field::PathChanged, this, &mesh_widget::SetNewPath);
}


void mesh_widget::UpdatePath()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::MeshComponent>()) destroy();

    const QString path =
        QString::fromStdString(Blainn::AssetManager::GetInstance()
                                   .GetMeshPath(*m_entity.GetComponent<Blainn::MeshComponent>().m_meshHandle)
                                   .string());
    m_path_input_field->SetPath(path);
}


void mesh_widget::SetNewPath(const QString &newPath)
{
    if (newPath.isEmpty()) return;

    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::MeshComponent>()) destroy();

    Blainn::Path path = ToString(newPath);
    auto &mesh = m_entity.GetComponent<Blainn::MeshComponent>();

    if (Blainn::AssetManager::GetInstance().HasMesh(path))
        mesh.m_meshHandle = Blainn::AssetManager::GetInstance().GetMesh(path);
    else
        mesh.m_meshHandle = Blainn::AssetManager::GetInstance().LoadMesh(
            path, Blainn::ImportMeshData::GetMeshData(Blainn::Engine::GetContentDirectory() / path));
}


void mesh_widget::DeleteComponent()
{
    if (m_entity.IsValid()) m_entity.RemoveComponent<Blainn::MeshComponent>();

    deleteLater();
}
} // namespace editor