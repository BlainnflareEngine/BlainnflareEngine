//
// Created by gorev on 30.09.2025.
//

#include "file-system/Texture.h"

namespace Blainn
{

Texture::Texture(const Path &path, const Microsoft::WRL::ComPtr<ID3D12Resource> &resource, TextureType type)
: FileSystemObject(path)
    , m_type(type), m_resource(resource)
{
}


Texture::~Texture()
{
}


void Texture::Move()
{
    FileSystemObject::Move();
}


void Texture::Delete()
{
    FileSystemObject::Delete();
}


void Texture::Copy()
{
    FileSystemObject::Copy();
}


ID3D12Resource &Texture::GetResource() const
{
    return *m_resource.Get();
}


} // namespace Blainn