//
// Created by gorev on 30.09.2025.
//

#include "subsystems/AssetManager.h"

#include "Engine.h"
#include "VGJS.h"
#include "file-system/Model.h"
#include "file-system/Texture.h"

namespace Blainn
{
AssetManager &AssetManager::GetInstance()
{
    static AssetManager instance;
    return instance;
}


void AssetManager::Init()
{
    BF_INFO("AssetManager Init");
    m_loader = eastl::make_unique<AssetLoader>();
    m_loader->Init();

    m_textures.reserve(512);
    m_materials.reserve(512);

    // TODO: create default texture
    m_textures.emplace_back(
        m_loader->LoadTexture(std::filesystem::current_path() / "Default.png", Texture::TextureType::ALBEDO));

    // TODO: create default material
    Material material = Material(std::filesystem::current_path() / "Default.mat", "Default");
    m_materials.emplace_back(material);
}


void AssetManager::Destroy()
{
    BF_INFO("AssetManager Destroy");
    m_loader->Destroy();
}


bool AssetManager::ModelExists(const Path &path)
{
    auto it = m_models.find(ToEASTLString(path.string()));
    return it != m_models.end();
}


eastl::shared_ptr<Model> AssetManager::GetModel(const Path &path)
{
    eastl::string pathStr = path.string().c_str();
    if (auto it = m_models.find(pathStr); it != m_models.end())
    {
        return it->second;
    }

    BF_ERROR("Model doesn't exist");
    return eastl::shared_ptr<Model>();
}


eastl::shared_ptr<Model> AssetManager::LoadModel(const Path &path, const ImportModelData &data)
{
    eastl::string pathStr = ToEASTLString(path.string());
    auto model = m_loader->ImportModel(path, data);
    m_models[pathStr] = model;
    return model;
}


bool AssetManager::HasTexture(const Path &path)
{
    return m_texturePaths.find(ToEASTLString(path.string())) != m_texturePaths.end();
}


eastl::shared_ptr<TextureHandle> AssetManager::GetTexture(const Path &path)
{
    if (auto it = m_texturePaths.find(ToEASTLString(path.string())); it != m_texturePaths.end())
        return eastl::make_shared<TextureHandle>(it->second);

    BF_ERROR("Texture doesn't exist. You should load it first.");
    return eastl::shared_ptr<TextureHandle>(nullptr);
}


eastl::shared_ptr<TextureHandle> AssetManager::LoadTexture(const Path &path, const Texture::TextureType type)
{
    unsigned int index = m_textures.size();
    m_texturePaths[ToEASTLString(path.string())] = index;

    // TODO: push back default texture
    auto str = "Shiiiii... I don't have such texture, here is default one, "
               "i will place some new texture to your index later "
               + std::to_string(index);
    BF_INFO(str)

    m_textures.emplace_back(GetDefaultTexture());
    vgjs::schedule([=]() { AddTextureWhenLoaded(path, index, type); });
    return eastl::make_shared<TextureHandle>(index);
}


eastl::shared_ptr<MaterialHandle> AssetManager::GetMaterial(const Path &path)
{
    if (auto it = m_materialPaths.find(ToEASTLString(path.string())); it != m_materialPaths.end())
        return eastl::make_shared<MaterialHandle>(it->second);

    BF_ERROR("Material doesn't exist. You should load it first.");
    return eastl::shared_ptr<MaterialHandle>(nullptr);
}


eastl::shared_ptr<MaterialHandle> AssetManager::LoadMaterial(const Path &path)
{
    unsigned int index = m_materials.size();
    m_materialPaths[ToEASTLString(path.string())] = index;

    // TODO: push back default material
    auto str = "I don't have that material, here is default one, "
               "i will place new material to your index later "
               + std::to_string(index);
    BF_INFO(str)

    m_materials.emplace_back(GetDefaultMaterial());
    vgjs::schedule([=]() { AddMaterialWhenLoaded(path, index); });
    return eastl::make_shared<MaterialHandle>(index);
}


Material &AssetManager::GetMaterialByIndex(unsigned int index)
{
    return m_materials[index];
}


Material &AssetManager::GetMaterialByHandle(const MaterialHandle &handle)
{
    unsigned int index = handle.GetIndex();
    if (index >= m_materials.size())
    {
        BF_FATAL("Failed to get material by index. Index is greater than m_materials size!");
        throw std::exception("Failed to get texture by index.");
    }

    return m_materials[index];
}


Texture &AssetManager::GetTextureByIndex(unsigned int index)
{
    return m_textures[index];
}


Texture &AssetManager::GetTextureByHandle(const TextureHandle &handle)
{
    unsigned int index = handle.GetIndex();
    if (index >= m_textures.size())
    {
        BF_FATAL("Failed to get texture by index. Index is greater than m_textures size!");
        throw std::exception("Failed to get texture by index.");
    }

    return m_textures[index];
}


void AssetManager::AddTextureWhenLoaded(const Path &path, const unsigned int index, const Texture::TextureType type)
{
    BF_INFO("Started loading texture.");
    m_textures[index] = m_loader->LoadTexture(path, type);
    auto str = "Placing texture to index " + std::to_string(index);
    BF_INFO(str);
}


void AssetManager::AddMaterialWhenLoaded(const Path &path, const unsigned int index)
{
    BF_INFO("Started loading material.");
    m_materials[index] = m_loader->LoadMaterial(path);
    auto str = "Placing material to index " + std::to_string(index);
    BF_INFO(str);
}


Texture &AssetManager::GetDefaultTexture()
{
    return m_textures[0];
}


Material &AssetManager::GetDefaultMaterial()
{
    return m_materials[0];
}

} // namespace Blainn