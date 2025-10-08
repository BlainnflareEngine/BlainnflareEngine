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

    // TODO: create default texture
    auto temp = Microsoft::WRL::ComPtr<ID3D12Resource>();
    m_defaultTexture = eastl::make_unique<Texture>(std::filesystem::current_path(), temp, Texture::TextureType::ALBEDO);
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


eastl::shared_ptr<Model> AssetManager::GetModel(const std::filesystem::path &path)
{
    eastl::string pathStr = path.string().c_str();
    if (auto it = m_models.find(pathStr); it != m_models.end())
    {
        return it->second;
    }

    BF_ERROR("Model doesn't exist");
    return eastl::shared_ptr<Model>();
}


eastl::shared_ptr<Model> AssetManager::LoadModel(const std::filesystem::path &path, const ImportModelData &data)
{
    eastl::string pathStr = ToEASTLString(path.string());
    auto model = m_loader->ImportModel(path, data);
    m_models[pathStr] = model;
    return model;
}


eastl::shared_ptr<TextureHandle> AssetManager::GetTexture(const std::filesystem::path &path)
{
    if (auto it = m_texture_paths.find(ToEASTLString(path.string())); it != m_texture_paths.end())
        return eastl::make_shared<TextureHandle>(it->second);


    unsigned int index = m_textures.size();
    m_texture_paths[ToEASTLString(path.string())] = index;
    // TODO: push back default texture
    auto str = "Shiiiii... I don't have such texture, here is default one, "
               "i will place some new texture to your index later "
               + std::to_string(index);
    BF_INFO(str)
    m_textures.emplace_back(*m_defaultTexture.get());
    vgjs::schedule([=]() { AddTextureWhenLoaded(path, index); });
    return eastl::make_shared<TextureHandle>(index);
}


Texture &AssetManager::GetTextureByIndex(unsigned int index)
{
    return m_textures[index];
}


Texture &AssetManager::GetTextureByHandle(const TextureHandle &handle)
{
    unsigned int index = handle.GetTextureIndex();
    if (index >= m_textures.size())
    {
        BF_FATAL("Failed to get texture by index. Index is greater than m_textures size!");
        throw std::exception("Failed to get texture by index.");
    }

    return m_textures[index];
}


void AssetManager::AddTextureWhenLoaded(const Path &path, int index)
{
    BF_INFO("Started loading texture");
    m_textures[index] = m_loader->LoadTexture(path);
    auto str = "Placing texture to index " + std::to_string(index);
    BF_INFO(str);
}

} // namespace Blainn