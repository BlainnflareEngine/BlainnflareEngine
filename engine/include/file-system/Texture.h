//
// Created by gorev on 30.09.2025.
//

#pragma once
#include <d3d12.h>
#include <wrl/client.h>

#include "FileSystemObject.h"
#include "TextureType.h"

namespace Blainn
{
    class Texture : public FileSystemObject
    {
    public:
        Texture() = default;
        Texture(const Path &path, TextureType type, uint32_t index/*, bool IsCubeMap = false*/);
        virtual ~Texture() override;

        Texture(Texture &&other) = delete;
        Texture(Texture &other) = delete;

        virtual void Move() override;
        virtual void Delete() override;
        virtual void Copy() override;

        ID3D12Resource* GetResource() const;
        TextureType GetType() const { return m_type; }
        UINT GetDescriptorOffset() const { return m_descriptorHeapOffset; }
        bool IsInitialized() const { return m_bIsInitialized; }

        bool IsLoaded() const { return m_bIsLoaded; }

        void SetDescriptorOffset(UINT newOffset);
        void DisposeUploaders();
    private:
        void Create(ID3D12GraphicsCommandList2 *cmdList, uint32_t index);
        // Only dds files supported
        void CreateCubemap(ID3D12GraphicsCommandList2 *cmdList, uint32_t index);
    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_resource = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadResource = nullptr;
        
        // cache handle (could be useful for default textures)
        CD3DX12_CPU_DESCRIPTOR_HANDLE localHandle = {};

        TextureType m_type = TextureType::NONE;
        UINT m_descriptorHeapOffset = 0u;
        
        bool m_bIsInitialized = false;
        bool m_bIsLoaded = false;
    };
} // namespace Blainn