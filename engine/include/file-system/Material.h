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

    void SetAlbedoColor(const Color& color);
    void SetNormalScale(float scale);
    void SetRoughnessScale(float roughness);
    void SetMetallicScale(float metallic);

    void SetShader(const eastl::string &shader);
    const eastl::string &GetShader() const;

    BOOL HasTexture(TextureType type) const;

private:
    eastl::shared_ptr<TextureHandle> m_albedoTexture;
    Color m_albedoColor = Color(1, 1, 1, 1);
    eastl::shared_ptr<TextureHandle> m_normalTexture;
    float m_normalScale = 1.0f;
    eastl::shared_ptr<TextureHandle> m_metallicTexture;
    float m_metallicScale = 0.5f;
    eastl::shared_ptr<TextureHandle> m_roughnessTexture;
    float m_roughnessScale = 0.5f;
    eastl::shared_ptr<TextureHandle> m_aoTexture;
    eastl::string m_shader;
};
} // namespace Blainn
