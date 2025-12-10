//
// Created by gorev on 30.09.2025.
//

#include "file-system/Material.h"
#include "file-system/TextureType.h"


Blainn::Material::Material(const Path &path, const eastl::string &shader)
    : FileSystemObject(path)
    , m_shader(shader)
{
}


Blainn::Material::~Material()
{
}


void Blainn::Material::Move()
{
    FileSystemObject::Move();
}


void Blainn::Material::Delete()
{
    FileSystemObject::Delete();
}


void Blainn::Material::Copy()
{
    FileSystemObject::Copy();
}


void Blainn::Material::SetTexture(const eastl::shared_ptr<TextureHandle> &textureHandle, TextureType type)
{
    switch (type)
    {
    case TextureType::NONE:
        BF_WARN("Trying to set none texture in " + m_path.string())
        break;

    case TextureType::ALBEDO:
        BF_INFO("Trying to set albedo texture in " + m_path.string())
        m_albedoTexture = textureHandle;
        break;

    case TextureType::NORMAL:
        BF_INFO("Trying to set normal texture in " + m_path.string())
        m_normalTexture = textureHandle;
        break;

    case TextureType::METALLIC:
        BF_INFO("Trying to set metallic texture in " + m_path.string())
        m_metallicTexture = textureHandle;
        break;

    case TextureType::ROUGHNESS:
        BF_INFO("Trying to set roughness texture in " + m_path.string())
        m_roughnessTexture = textureHandle;
        break;

    case TextureType::AO:
        BF_INFO("Trying to set AO texture in " + m_path.string())
        m_aoTexture = textureHandle;
        break;

    case TextureType::OTHER:
        BF_INFO("Trying to set other texture in " + m_path.string())
        break;

    default:
        BF_ERROR("Fuck you!");
        break;
    }
}


Blainn::TextureHandle &Blainn::Material::GetTextureHandle(TextureType type)
{
    switch (type)
    {

    case TextureType::NONE:
        BF_FATAL("There is no texture handle for this texture type (NONE).");
        throw std::runtime_error("Invalid texture type: NONE");

    case TextureType::ALBEDO:
        return *m_albedoTexture;

    case TextureType::NORMAL:
        return *m_normalTexture;

    case TextureType::METALLIC:
        return *m_metallicTexture;

    case TextureType::ROUGHNESS:
        return *m_roughnessTexture;

    case TextureType::AO:
        return *m_aoTexture;

    case TextureType::OTHER:
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


BOOL Blainn::Material::HasTexture(const TextureType type) const
{
    switch (type)
    {
    case TextureType::ALBEDO:
        return m_albedoTexture != nullptr;
    case TextureType::NORMAL:
        return m_normalTexture != nullptr;
    case TextureType::METALLIC:
        return m_metallicTexture != nullptr;
    case TextureType::ROUGHNESS:
        return m_roughnessTexture != nullptr;
    case TextureType::AO:
        return m_aoTexture != nullptr;
    default:
        return FALSE;
    }
}