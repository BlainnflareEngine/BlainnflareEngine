//
// Created by gorev on 30.09.2025.
//

#include "subsystems/AssetManager.h"

#include "Engine.h"
#include "ImportModelData.h"
#include "VGJS.h"
#include "file-system/Material.h"
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
    m_materials.reserve(64);
    m_meshes.reserve(64);

    // TODO: create default texture
    m_textures.emplace(
        m_loader->LoadTexture(std::filesystem::current_path() / "Default.png", TextureType::ALBEDO));

    // TODO: create default material
    Material material = Material(std::filesystem::current_path() / "Default.mat", "Default");
    m_materials.emplace(eastl::make_shared<Material>(material));

    // TODO: create default mesh
    // m_meshes.emplace_back();
}


void AssetManager::Destroy()
{
    BF_INFO("AssetManager Destroy");
    m_loader->Destroy();
}


bool AssetManager::MeshExists(const Path &path)
{
    auto it = m_meshPaths.find(ToEASTLString(path.string()));
    return it != m_meshPaths.end();
}


eastl::shared_ptr<MeshHandle> AssetManager::GetMesh(const Path &path)
{
    eastl::string pathStr = path.string().c_str();
    if (auto it = m_meshPaths.find(pathStr); it != m_meshPaths.end())
    {
        return eastl::make_shared<MeshHandle>(it->second.index);
    }

    BF_ERROR("This model doesn't imported yet. You should import it first. {0}", path.string());

    // TODO: return default mesh
    return eastl::shared_ptr<MeshHandle>(0);
}


eastl::shared_ptr<MeshHandle> AssetManager::LoadMesh(const Path &path, const ImportModelData &data)
{
    unsigned int index = m_meshes.size();
    m_meshPaths[ToEASTLString(path.string())] = AssetData{index, 1};


    BF_INFO("I don't have such model yet, here is default one, "
            "i will place your model to your index later. Index - {0}",
            index);

    m_meshes.emplace(eastl::make_shared<Model>(GetDefaultModel()));

    auto model = m_loader->ImportModel(path, data);
    return eastl::make_shared<MeshHandle>(index);
}


Model &AssetManager::GetMeshByIndex(const unsigned int index)
{
    return *m_meshes[index];
}


Model &AssetManager::GetMeshByHandle(const MeshHandle &handle)
{
    return *m_meshes[handle.GetIndex()];
}


bool AssetManager::HasTexture(const Path &path)
{
    return m_texturePaths.find(ToEASTLString(path.string())) != m_texturePaths.end();
}


eastl::shared_ptr<TextureHandle> AssetManager::GetTexture(const Path &path)
{
    if (auto it = m_texturePaths.find(ToEASTLString(path.string())); it != m_texturePaths.end())
        return eastl::make_shared<TextureHandle>(it->second.index);

    BF_ERROR("Texture doesn't exist. You should load it first. Texture - {0}", path.string());
    return eastl::shared_ptr<TextureHandle>(0);
}


eastl::shared_ptr<TextureHandle> AssetManager::LoadTexture(const Path &path, const TextureType type)
{
    unsigned int index = m_textures.size();
    m_texturePaths[ToEASTLString(path.string())] = AssetData{index, 1};

    BF_INFO("Shiiiii... I don't have such texture, here is default one, "
            "i will place some new texture to your index later. Index ={0}.",
            index);

    m_textures.emplace(eastl::make_shared<Texture>(GetDefaultTexture()));
    vgjs::schedule([=]() { AddTextureWhenLoaded(path, index, type); });
    return eastl::make_shared<TextureHandle>(index);
}


eastl::shared_ptr<MaterialHandle> AssetManager::GetMaterial(const Path &path)
{
    if (auto it = m_materialPaths.find(ToEASTLString(path.string())); it != m_materialPaths.end())
        return eastl::make_shared<MaterialHandle>(it->second.index);

    BF_ERROR("Material doesn't exist. You should load it first. Path - {0}", path.string());
    return eastl::shared_ptr<MaterialHandle>(0);
}


eastl::shared_ptr<MaterialHandle> AssetManager::LoadMaterial(const Path &path)
{
    unsigned int index = m_materials.size();
    m_materialPaths[ToEASTLString(path.string())] = AssetData{index, 1};

    // TODO: push back default material
    auto str = "I don't have that material, here is default one, "
               "i will place new material to your index later "
               + std::to_string(index);
    BF_INFO(str)

    m_materials.emplace(eastl::make_shared<Material>(GetDefaultMaterial()));
    vgjs::schedule([=]() { AddMaterialWhenLoaded(path, index); });
    return eastl::make_shared<MaterialHandle>(index);
}


Material &AssetManager::GetMaterialByIndex(unsigned int index)
{
    return *m_materials[index];
}


Material &AssetManager::GetMaterialByHandle(const MaterialHandle &handle)
{
    unsigned int index = handle.GetIndex();
    if (index >= m_materials.size())
    {
        BF_FATAL("Failed to get material by index. Index is greater than m_materials size!");
        throw std::exception("Failed to get texture by index.");
    }

    return *m_materials[index];
}


Texture &AssetManager::GetTextureByIndex(unsigned int index)
{
    return *m_textures[index];
}


Texture &AssetManager::GetTextureByHandle(const TextureHandle &handle)
{
    unsigned int index = handle.GetIndex();
    if (index >= m_textures.size())
    {
        BF_FATAL("Failed to get texture by index. Index is greater than m_textures size!");
        throw std::exception("Failed to get texture by index.");
    }

    return *m_textures[index];
}


void AssetManager::AddTextureWhenLoaded(const Path &path, const unsigned int index, const TextureType type)
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


void AssetManager::AddModelWhenLoaded(const Path &path, const unsigned int index, const ImportModelData data)
{
    BF_INFO("Started loading model.");
    m_meshes[index] = m_loader->ImportModel(path, data);
    auto str = "Placing model to index " + std::to_string(index);
    BF_INFO(str);
}


Texture &AssetManager::GetDefaultTexture()
{
    return *m_textures[0];
}


Material &AssetManager::GetDefaultMaterial()
{
    return *m_materials[0];
}


Model &AssetManager::GetDefaultModel()
{
    return *m_meshes[0];
}


void AssetManager::IncreaseTextureRefCount(const unsigned int index)
{
    for (auto &[key, value] : m_texturePaths)
        if (value.index == index) ++value.refCount;
}


void AssetManager::IncreaseMaterialRefCount(const unsigned int index)
{
    for (auto &[key, value] : m_materialPaths)
        if (value.index == index) ++value.refCount;
}


void AssetManager::IncreaseMeshRefCount(const unsigned int index)
{
    for (auto &[key, value] : m_meshPaths)
        if (value.index == index) ++value.refCount;
}


void AssetManager::DecreaseTextureRefCount(const unsigned int index)
{
    for (auto &[key, value] : m_texturePaths)
        if (value.index == index) --value.refCount;

    // TODO: remove asset if refCount == 0
}


void AssetManager::DecreaseMaterialRefCount(const unsigned int index)
{
    for (auto &[key, value] : m_materialPaths)
        if (value.index == index) --value.refCount;

    // TODO: remove asset if refCount == 0
}


void AssetManager::DecreaseMeshRefCount(const unsigned int index)
{
    for (auto &[key, value] : m_meshPaths)
        if (value.index == index) --value.refCount;

    // TODO: remove asset if refCount == 0
}

} // namespace Blainn