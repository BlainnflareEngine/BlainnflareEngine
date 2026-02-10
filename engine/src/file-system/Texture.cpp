//
// Created by gorev on 30.09.2025.
//

#include "Engine.h"
#include "file-system/Texture.h"
#include "file-system/TextureType.h"

#include "Render/Device.h"
#include "Render/CommandQueue.h"
// d3d12.h must be included before DirectXTex to allow support of Direct3D 12
#include <DirectXTex/DirectXTex/DirectXTex.h>

namespace Blainn
{
Texture::Texture(const Path &path, TextureType type, uint32_t index/*, bool IsCubeMap*/)
    : FileSystemObject(path)
    , m_type(type)
    {
        const auto& cmdQueue = Device::GetInstance().GetCommandQueue();
        const auto& cmdAlloc = cmdQueue->GetCommandAllocator();
        const auto& cmdList = cmdQueue->GetCommandList(cmdAlloc.Get());

        [[likely]]
        if (!(type == TextureType::CUBEMAP))
            Create(cmdList.Get(), index);
        else
            CreateCubemap(cmdList.Get(), index);

        cmdQueue->ExecuteCommandList(cmdList);
        cmdQueue->Flush();

        m_bIsLoaded = m_bIsInitialized;
    }

    Texture::~Texture()
    {
        m_bIsLoaded = false;
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

    void Texture::Create(ID3D12GraphicsCommandList2 *cmdList, uint32_t index)
    {
        // Gpu stuff
        auto& device = Device::GetInstance();
        auto comDevice = device.GetDevice2();

        ScratchImage image;
        ScratchImage mipChain;

        if (FAILED(LoadFromWICFile((Engine::GetContentDirectory() / m_path).wstring().c_str(), WIC_FLAGS_FORCE_RGB, nullptr, image)))
        {
            BF_ERROR("Failed to load texture: {}", (char*)m_path.u8string().c_str());
            return;
        }

        if (FAILED(GenerateMipMaps(*image.GetImages(), TEX_FILTER_BOX, 0, mipChain)))
        {
            BF_ERROR("Failed to generate mip map: {}", (char*)m_path.u8string().c_str());
        }

        // create resource
        const auto &chainBase = *mipChain.GetImages();

        D3D12_RESOURCE_DESC texDesc = {};
        texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        texDesc.Width = (UINT)chainBase.width;
        texDesc.Height = (UINT)chainBase.height;
        texDesc.DepthOrArraySize = (UINT16)1u;
        texDesc.MipLevels = (UINT16)mipChain.GetImageCount();
        texDesc.Format = chainBase.format;
        texDesc.SampleDesc.Count = 1u;
        texDesc.SampleDesc.Quality = 0u;
        texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;
        texDesc.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;

        CD3DX12_HEAP_PROPERTIES heapProps{D3D12_HEAP_TYPE_DEFAULT};

        if (FAILED(comDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &texDesc,
                                                      D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
                                                      IID_PPV_ARGS(&m_resource))))
        {
            BF_ERROR("Failed to create texture: {}", (char*)m_path.u8string().c_str());
            return;
        }
        std::wstring name = m_path.wstring() + L" texture";
        m_resource->SetName(name.c_str());

        // collect subresource data
        eastl::vector<D3D12_SUBRESOURCE_DATA> subresources((int)mipChain.GetImageCount());
        for (int i = 0; i < (int)mipChain.GetImageCount(); i++)
        {
            const auto img = mipChain.GetImage(i, 0, 0);
            auto &subresource = subresources[i];
            subresource.pData = img->pixels;
            subresource.RowPitch = (LONG_PTR)img->rowPitch;
            subresource.SlicePitch = (LONG_PTR)img->slicePitch;
        }

        // intermediate upload buffer
        CD3DX12_HEAP_PROPERTIES heapProps2{D3D12_HEAP_TYPE_UPLOAD};
        const UINT64 uploadBufferSize = GetRequiredIntermediateSize(m_resource.Get(), 0, static_cast<uint32_t>(subresources.size()));
        const CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

        if (FAILED(comDevice->CreateCommittedResource(&heapProps2, D3D12_HEAP_FLAG_NONE, &resourceDesc,
                                                      D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                      IID_PPV_ARGS(&m_uploadResource))))
        {
            BF_ERROR("Failed to create upload buffer for: {}", (char*)m_path.u8string().c_str());
            m_resource = nullptr;
            return;
        }
        std::wstring uploadname = m_path.wstring() + L" upload buffer";
        m_uploadResource->SetName(uploadname.c_str());

        // write commands to copy data to upload texture (copying each subresource)
        UpdateSubresources(cmdList, m_resource.Get(), m_uploadResource.Get(), (UINT64)0u, 0u,
                           static_cast<uint32_t>(subresources.size()), subresources.data());

        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
            m_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
        cmdList->ResourceBarrier(1u, &barrier);

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        ZeroMemory(&srvDesc, sizeof(srvDesc));
        srvDesc.Format = m_resource->GetDesc().Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2D.MostDetailedMip = 0u;
        srvDesc.Texture2D.MipLevels = m_resource->GetDesc().MipLevels;
        srvDesc.Texture2D.PlaneSlice = 0;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

        UINT texturePlacementOffset = TextureSrvHeapStartIndex /*+ (static_cast<UINT>(m_type) - 1u) * MAX_TEX_OF_TYPE*/ + index;//textureTableOffset;
        m_descriptorHeapOffset = texturePlacementOffset;

        auto srvCpuStart = device.GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
        auto cbvSrvUavDescriptorSize = device.GetDescriptorHandleIncrementSize();

        localHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, texturePlacementOffset, cbvSrvUavDescriptorSize);
        device.CreateShaderResourceView(m_resource.Get(), &srvDesc, localHandle);

        m_bIsInitialized = true;
    }

    void Texture::CreateCubemap(ID3D12GraphicsCommandList2 *cmdList, uint32_t index)
    {
        (void)index;
        //assert(m_path.extension() == ".dds");
        if (!(m_path.extension() == ".dds"))
        {
            BF_ERROR("Cubemap has to be a dds file");
            return;
        }
        // Gpu stuff
        auto& device = Device::GetInstance();
        const auto& comDevice = device.GetDevice2();

        ScratchImage image;
        ThrowIfFailed(LoadFromDDSFile((Engine::GetContentDirectory() / m_path).wstring().c_str(), DDS_FLAGS_FORCE_RGB, nullptr, image));

        const TexMetadata &metadata = image.GetMetadata();

        if (metadata.arraySize != 6)
        {
            BF_ERROR("Cubemap must have 6 textures!\n");
            return;
        }

        ScratchImage mipChain;
        if (metadata.mipLevels == 1)
        {
            ThrowIfFailed(GenerateMipMaps(*image.GetImages(), TEX_FILTER_DEFAULT, 0, mipChain));
        }
        else
        {
            mipChain = std::move(image);
        }

        const TexMetadata &mipMetadata = mipChain.GetMetadata();

        D3D12_RESOURCE_DESC texDesc = {};
        texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        texDesc.Alignment = 0;
        texDesc.Width = static_cast<UINT>(mipMetadata.width);
        texDesc.Height = static_cast<UINT>(mipMetadata.height);
        texDesc.DepthOrArraySize = static_cast<UINT16>(mipMetadata.arraySize);
        texDesc.MipLevels = static_cast<UINT16>(mipMetadata.mipLevels);
        texDesc.Format = mipMetadata.format;
        texDesc.SampleDesc.Count = 1;
        texDesc.SampleDesc.Quality = 0;
        texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        texDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

        ThrowIfFailed(comDevice->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr,
                                                         IID_PPV_ARGS(&m_resource)));

        std::vector<D3D12_SUBRESOURCE_DATA> subresources;
        for (int face = 0; face < 6; ++face)
        {
            for (int mip = 0; mip < (int)metadata.mipLevels; ++mip)
            {
                const Image *img = mipChain.GetImage(mip, face, 0);
                D3D12_SUBRESOURCE_DATA s = {};
                s.pData = img->pixels;
                s.RowPitch = img->rowPitch;
                s.SlicePitch = img->slicePitch;
                subresources.push_back(s);
            }
        }

        UINT64 uploadBufferSize =
            GetRequiredIntermediateSize(m_resource.Get(), 0, static_cast<UINT>(subresources.size()));

        CD3DX12_HEAP_PROPERTIES heapPropsUpload(D3D12_HEAP_TYPE_UPLOAD);
        CD3DX12_RESOURCE_DESC bufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);

        ThrowIfFailed(comDevice->CreateCommittedResource(&heapPropsUpload, D3D12_HEAP_FLAG_NONE, &bufferDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr,
                                                         IID_PPV_ARGS(&m_uploadResource)));

        UpdateSubresources(cmdList, m_resource.Get(), m_uploadResource.Get(), (UINT64)0u, 0u, static_cast<UINT>(subresources.size()), subresources.data());

        CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(m_resource.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);

        cmdList->ResourceBarrier(1, &barrier);

        D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
        srvDesc.Format = mipMetadata.format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.TextureCube.MipLevels = static_cast<UINT>(mipMetadata.mipLevels);
        srvDesc.TextureCube.MostDetailedMip = 0;
        srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;

        auto srvCpuStart = device.GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
        auto cbvSrvUavDescriptorSize = device.GetDescriptorHandleIncrementSize();

        localHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, SkyboxSrvHeapStartIndex, cbvSrvUavDescriptorSize);
        // index for CubeMap is always constant and defined at init time
        device.CreateShaderResourceView(m_resource.Get(), &srvDesc, localHandle);

        m_bIsInitialized = true;
    }

    void Texture::DisposeUploaders()
    {
        m_uploadResource = nullptr;
    }

    void Texture::SetDescriptorOffset(UINT newOffset)
    {
        m_descriptorHeapOffset = newOffset;
    }

} // namespace Blainn
