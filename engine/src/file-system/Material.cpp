//
// Created by gorev on 30.09.2025.
//

#include "file-system/Material.h"


Blainn::Material::Material(const Path &path, const eastl::string &shader)
    : FileSystemObject(path)
    , m_shader(shader)
{
}


void Blainn::Material::SetTexture(const eastl::shared_ptr<TextureHandle> &textureHandle, Texture::TextureType type)
{
    switch (type)
    {
    case Texture::TextureType::NONE:
        BF_WARN("Trying to set none texture in " + m_path.string())
        break;

    case Texture::TextureType::ALBEDO:
        BF_INFO("Trying to set albedo texture in " + m_path.string())
        m_albedoTexture = textureHandle;
        break;

    case Texture::TextureType::NORMAL:
        BF_INFO("Trying to set normal texture in " + m_path.string())
        m_normalTexture = textureHandle;
        break;

    case Texture::TextureType::METALLIC:
        BF_INFO("Trying to set metallic texture in " + m_path.string())
        m_metallicTexture = textureHandle;
        break;

    case Texture::TextureType::ROUGHNESS:
        BF_INFO("Trying to set roughness texture in " + m_path.string())
        m_roughnessTexture = textureHandle;
        break;

    case Texture::TextureType::AO:
        BF_INFO("Trying to set AO texture in " + m_path.string())
        m_aoTexture = textureHandle;
        break;

    case Texture::TextureType::OTHER:
        BF_INFO("Trying to set other texture in " + m_path.string())
        break;
    }
}


Blainn::TextureHandle &Blainn::Material::GetTextureHandle(Texture::TextureType type)
{
    switch (type)
    {

    case Texture::TextureType::NONE:
        BF_FATAL("There is no texture handle for this texture type (NONE).");
        throw std::runtime_error("Invalid texture type: NONE");

    case Texture::TextureType::ALBEDO:
        return *m_albedoTexture;

    case Texture::TextureType::NORMAL:
        return *m_normalTexture;

    case Texture::TextureType::METALLIC:
        return *m_metallicTexture;

    case Texture::TextureType::ROUGHNESS:
        return *m_roughnessTexture;

    case Texture::TextureType::AO:
        return *m_aoTexture;

    case Texture::TextureType::OTHER:
        BF_WARN("There is no texture handle for this texture type (OTHER).");
        return *m_albedoTexture;

    default:
        throw std::runtime_error("Invalid texture type: default");
    }
}


void Blainn::Material::SetShader(const eastl::string &shader)
{
    m_shader = shader;
}


const eastl::string &Blainn::Material::GetShader() const
{
    return m_shader;
}