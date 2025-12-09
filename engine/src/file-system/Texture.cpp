//
// Created by gorev on 30.09.2025.
//

#include "file-system/Texture.h"
#include "file-system/TextureType.h"

#include "Render/Device.h"
#include "Render/CommandQueue.h"

#include <DirectXTK12/Inc/DDSTextureLoader.h>
#include <DirectXTK12/Inc/ResourceUploadBatch.h>

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

    ID3D12Resource* Texture::GetResource() const
    {
        return m_resource.Get();
    }

    void Texture::DisposeUploaders()
    {
        m_uploadHeap = nullptr;
    }

} // namespace Blainn