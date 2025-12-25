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
    m_path_input = new path_input_field("Mesh", formats::supported3DFormats, this);

    m_material_input = new path_input_field("Material", {formats::materialFormat}, this);

    layout()->addWidget(m_path_input);
    layout()->addWidget(m_material_input);

    BlockSignals(true);
    UpdatePath();
    UpdateMaterial();
    BlockSignals(false);

    connect(m_path_input, &path_input_field::PathChanged, this, &mesh_widget::SetNewPath);
    connect(m_material_input, &path_input_field::PathChanged, this, &mesh_widget::SetNewMaterial);
    connect(m_path_input, &path_input_field::PathChanged, this, &mesh_widget::SetNewPath);
    connect(m_material_input, &path_input_field::PathChanged, this, &mesh_widget::SetNewMaterial);
}


void mesh_widget::UpdatePath()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::MeshComponent>()) destroy();

    const QString path =
        QString::fromStdString(Blainn::AssetManager::GetInstance()
                                   .GetMeshPath(*m_entity.GetComponent<Blainn::MeshComponent>().MeshHandle)
                                   .string());
    m_path_input->SetPath(path);
}


void mesh_widget::UpdateMaterial()
{
    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::MeshComponent>()) destroy();

    const QString path =
        QString::fromStdString(Blainn::AssetManager::GetInstance()
                                   .GetMaterialPath(*m_entity.GetComponent<Blainn::MeshComponent>().MaterialHandle)
                                   .string());

    m_material_input->SetPath(path);
}


void mesh_widget::SetNewPath(const QString &oldPath, const QString &newPath)
{
    if (newPath.isEmpty()) return;

    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::MeshComponent>()) destroy();

    Blainn::Path path = ToString(newPath);
    auto &mesh = m_entity.GetComponent<Blainn::MeshComponent>();

    if (Blainn::AssetManager::GetInstance().HasMesh(path))
        mesh.MeshHandle = Blainn::AssetManager::GetInstance().GetMesh(path);
    else
        mesh.MeshHandle = Blainn::AssetManager::GetInstance().LoadMesh(
            path, Blainn::ImportMeshData::GetMeshData(Blainn::Engine::GetContentDirectory() / path));
}


void mesh_widget::SetNewMaterial(const QString &oldPath, const QString &newPath)
{
    if (newPath.isEmpty()) return;

    if (!m_entity.IsValid() || !m_entity.HasComponent<Blainn::MeshComponent>()) destroy();

    Blainn::Path path = ToString(newPath);
    auto &mesh = m_entity.GetComponent<Blainn::MeshComponent>();

    if (Blainn::AssetManager::GetInstance().HasMaterial(path))
        mesh.MaterialHandle = Blainn::AssetManager::GetInstance().GetMaterial(path);
    else mesh.MaterialHandle = Blainn::AssetManager::GetInstance().LoadMaterial(path);
}


void mesh_widget::DeleteComponent()
{
    if (m_entity.IsValid()) m_entity.RemoveComponent<Blainn::MeshComponent>();

    deleteLater();
}


void mesh_widget::BlockSignals(bool block)
{
    m_path_input->blockSignals(block);
    m_material_input->blockSignals(block);
}
} // namespace editor