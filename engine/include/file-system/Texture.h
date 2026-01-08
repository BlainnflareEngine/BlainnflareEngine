//
// Created by gorev on 30.09.2025.
//

#pragma once
#include "FileSystemObject.h"
#include "TextureType.h"

#include <d3d12.h>
#include <wrl/client.h>

namespace Blainn
{
    class Texture : public FileSystemObject
    {
    public:
        Texture(const Path &path, const Microsoft::WRL::ComPtr<ID3D12Resource> &resource, TextureType type);
        virtual ~Texture() override;

        virtual void Move() override;
        virtual void Delete() override;
        virtual void Copy() override;

        ID3D12Resource* GetResource() const;
        void SetTextureDescriptorOffset(UINT newOffset);
        UINT GetTextureDescOffset() const { return m_textureDescriptorOffset; }
        TextureType GetTextureType() const { return m_type; }

        void DisposeUploaders();

    private:
        UINT m_textureDescriptorOffset = 0u;
        TextureType m_type;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
        Microsoft::WRL::ComPtr<ID3D12Resource> m_uploadHeap = nullptr;
    };
} // namespace Blainn