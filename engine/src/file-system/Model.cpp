//
// Created by gorev on 30.09.2025.
//

#include "file-system/Model.h"

namespace Blainn
{


Model::Model(const Path &absolutPath)
    : FileSystemObject(absolutPath)
{
    m_meshes.reserve(4);
}


Model::Model(const Model &other, const Path &absolutPath)
    : FileSystemObject(absolutPath)
{
    m_meshes = other.m_meshes;
}


Model::Model(Model &&other, const Path &absolutPath) noexcept
    : FileSystemObject(absolutPath)
{
    m_meshes = std::move(other.m_meshes);
}


Model::~Model()
{
}


void Model::Copy()
{
    FileSystemObject::Copy();
}


void Model::Delete()
{
    FileSystemObject::Delete();
}


void Model::Move()
{
    FileSystemObject::Move();
}


eastl::vector<MeshData> &Model::GetMeshes()
{
    return m_meshes;
}


void Model::SetMeshes(const eastl::vector<MeshData> &meshes)
{
    m_meshes = meshes;
}
} // namespace Blainn