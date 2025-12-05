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

    ID3D12Resource &Texture::GetResource() const
    {
        return *m_resource.Get();
    }

    void Texture::CreateBufferResources()
    {
        auto device = Device::GetInstance().GetDevice2().Get();
        auto commandQueue = Device::GetInstance().GetCommandQueue();
     
        ResourceUploadBatch upload(device);
        upload.Begin();

        ThrowIfFailed(CreateDDSTextureFromFile(device, upload, m_path.wstring().c_str(), m_resource.ReleaseAndGetAddressOf()));

       /* auto desc = CD3DX12_RESOURCE_DESC(
            D3D12_RESOURCE_DIMENSION_TEXTURE2D, D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT, c_texture_size,
            c_texture_size, 1, 1, DXGI_FORMAT_R8G8_SNORM, 1, 0, D3D12_TEXTURE_LAYOUT_UNKNOWN, D3D12_RESOURCE_FLAG_NONE);

        CD3DX12_HEAP_PROPERTIES defaultHeapProperties(D3D12_HEAP_TYPE_DEFAULT);

        ThrowIfFailed(device->CreateCommittedResource(&defaultHeapProperties, D3D12_HEAP_FLAG_NONE, &desc,
                                                          D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
                                                          IID_PPV_ARGS(&m_resource)));

        D3D12_SUBRESOURCE_DATA initData = {mydata, c_texture_size * 2, 0};
        upload.Upload(tex.Get(), 0, &initData, 1);

        upload.Transition(tex.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);*/


        // Upload the resources to the GPU.
        auto finish = upload.End(commandQueue->GetCommandQueue().Get());

        // Wait for the upload thread to terminate
        finish.wait();
    }

    void Texture::DisposeUploaders()
    {
        m_uploadHeap = nullptr;
    }

} // namespace Blainn