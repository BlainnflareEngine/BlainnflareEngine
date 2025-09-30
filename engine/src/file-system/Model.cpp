//
// Created by gorev on 30.09.2025.
//

#include "file-system/Model.h"

namespace Blainn
{


Model::Model(const Path &path)
    : FileSystemObject(path)
{
    m_meshes.reserve(4);
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