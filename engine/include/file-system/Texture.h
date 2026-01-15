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
        Texture(const Path &path, TextureType type, uint32_t& textureTableOffset);
        virtual ~Texture() override;

        virtual void Move() override;
        virtual void Delete() override;
        virtual void Copy() override;

        ID3D12Resource* GetResource() const;
        TextureType GetType() const { return m_type; }
        UINT GetDescriptorOffset() const { return m_descriptorHeapOffset; }
        bool IsInitialized() const { return m_bIsInitialized; }

        void SetDescriptorOffset(UINT newOffset);
        void DisposeUploaders();
    private:
        void CreateGPUResources(ID3D12GraphicsCommandList2 *cmdList, uint32_t &textureTableOffset);
    private:
        Microsoft::WRL::ComPtr<ID3D12Resource> m_resource = nullptr;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadResource = nullptr;
        
        TextureType m_type = TextureType::NONE;
        UINT m_descriptorHeapOffset = 0u;
        
        bool m_bIsInitialized = false;
    };
} // namespace Blainn