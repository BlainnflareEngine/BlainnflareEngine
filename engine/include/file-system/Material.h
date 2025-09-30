//
// Created by gorev on 30.09.2025.
//

#pragma once
#include "FileSystemObject.h"
#include "handles/Handle.h"
#include "Texture.h"


namespace Blainn
{
class Material : public FileSystemObject
{
public:
    Material(const Path& path);
    virtual ~Material() override;

    virtual void Move() override;
    virtual void Delete() override;
    virtual void Copy() override;

    void SetTexture(const TextureHandle &textureHandle, Texture::TextureType type);
    TextureHandle& GetTextureHandle(Texture::TextureType type);

private:
    TextureHandle m_albedoTexture;
    TextureHandle m_normalTexture;
    TextureHandle m_metallicTexture;
    TextureHandle m_roughnessTexture;
    TextureHandle m_aoTexture;
};
} // namespace Blainn
