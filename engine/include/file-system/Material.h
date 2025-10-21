//
// Created by gorev on 30.09.2025.
//

#pragma once
#include "FileSystemObject.h"

namespace Blainn
{
struct TextureHandle;
enum class TextureType;

class Material : public FileSystemObject
{
public:
    Material(const Path &path, const eastl::string &shader);
    virtual ~Material() override;

    virtual void Move() override;
    virtual void Delete() override;
    virtual void Copy() override;

    void SetTexture(const eastl::shared_ptr<TextureHandle> &textureHandle, TextureType type);
    TextureHandle &GetTextureHandle(TextureType type);

    void SetShader(const eastl::string &shader);
    const eastl::string &GetShader() const;

    BOOL HasTexture(TextureType type) const;

private:
    eastl::shared_ptr<TextureHandle> m_albedoTexture;
    eastl::shared_ptr<TextureHandle> m_normalTexture;
    eastl::shared_ptr<TextureHandle> m_metallicTexture;
    eastl::shared_ptr<TextureHandle> m_roughnessTexture;
    eastl::shared_ptr<TextureHandle> m_aoTexture;
    eastl::string m_shader;
};
} // namespace Blainn
