//
// Created by gorev on 30.09.2025.
//

#pragma once
#include "FileSystemObject.h"
#include <d3d12.h>
#include <wrl/client.h>

namespace Blainn
{

class Texture : public FileSystemObject
{
public:
    enum class TextureType
    {
        NONE = 0,
        ALBEDO = 1,
        NORMAL = 2,
        METALLIC = 3,
        ROUGHNESS = 4,
        AO = 5,
        OTHER = 6,
    };

    Texture(const Path &path, const Microsoft::WRL::ComPtr<ID3D12Resource> &resource, TextureType type);
    virtual ~Texture() override;

    virtual void Move() override;
    virtual void Delete() override;
    virtual void Copy() override;

    ID3D12Resource &GetResource() const;

private:
    TextureType m_type;
    Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
};

} // namespace Blainn