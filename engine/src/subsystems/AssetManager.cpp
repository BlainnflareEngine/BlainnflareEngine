//
// Created by gorev on 30.09.2025.
//

#include "Subsystems/AssetManager.h"

#include "Engine.h"
#include "ImportAssetData.h"
#include "VGJS.h"
#include "File-System/Material.h"
#include "File-System/Model.h"
#include "File-System/Texture.h"

#include "Render/PrebuiltEngineMeshes.h"

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

    m_textures.reserve(MAX_TEXTURES);
    m_materials.reserve(MAX_MATERIALS);
    m_meshes.reserve(MAX_MESHES);

#pragma region LoadDefaultResource
    // TODO: create default texture
    m_textures.emplace(m_loader->LoadTexture(Engine::GetContentDirectory() / "Textures\\Default.dds", TextureType::ALBEDO));

    // TODO: create default material
    Material material = Material(Engine::GetContentDirectory() / "Materials\\Default.mat", "Default");
    m_materials.emplace(eastl::make_shared<Material>(material));

   /* auto &device = Device::GetInstance();
    auto commandQueue = device.GetCommandQueue();
    auto cmdList = commandQueue->GetDefaultCommandList();*/

    auto defaultMeshData = PrebuiltEngineMeshes::CreateBox(1.f, 1.f, 1.f);
    Model model;
    model.SetMeshes({defaultMeshData});
    model.CreateBufferResources();
    m_loader->CreateModelGPUResources(model);

    m_meshes.emplace(eastl::make_shared<Model>(model));
#pragma endregion LoadDefaultResource
}


void AssetManager::Destroy()
{
    BF_INFO("AssetManager Destroy");
    m_loader->Destroy();
}


bool AssetManager::HasMesh(const Path &relativePath)
{
    return m_meshPaths.contains(ToEASTLString(relativePath.string()));
}


eastl::shared_ptr<MeshHandle> AssetManager::GetMesh(const Path &relativePath)
{
    eastl::string pathStr = relativePath.string().c_str();
    if (auto it = m_meshPaths.find(pathStr); it != m_meshPaths.end())
    {
        return eastl::make_shared<MeshHandle>(it->second.index);
    }

    BF_ERROR("This model isn't imported yet. You should import it first. {0}", relativePath.string());

    // TODO: return default mesh
    return GetDefaultMesh();
}


eastl::shared_ptr<MeshHandle> AssetManager::GetDefaultMesh()
{
    return eastl::make_shared<MeshHandle>(0);
}


eastl::shared_ptr<MeshHandle> AssetManager::LoadMesh(const Path &relativePath, const ImportMeshData &data)
{
    int index = m_meshes.size();
    m_meshPaths[ToEASTLString(relativePath.string())] = AssetData{index, 1};


    BF_INFO("I don't have such model yet, here is default one, "
            "i will place your model to your index later. Index - {0}",
            index);

    m_meshes.emplace(eastl::make_shared<Model>(GetDefaultModel(), relativePath));

    vgjs::schedule([=]() { AddMeshWhenLoaded(relativePath, index, data); });
    return eastl::make_shared<MeshHandle>(index);
}


Model &AssetManager::GetMeshByIndex(const unsigned int index)
{
    if (m_meshes[index]->IsLoaded()) return *m_meshes[index];
    else return GetDefaultMesh()->GetMesh();
}


Model &AssetManager::GetMeshByHandle(const MeshHandle &handle)
{
    return GetMeshByIndex(handle.GetIndex());
}


Path AssetManager::GetMeshPath(const MeshHandle &handle)
{
    return m_meshes[handle.GetIndex()]->GetPath();
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
    return eastl::make_shared<TextureHandle>(0);
}


eastl::shared_ptr<TextureHandle> AssetManager::LoadTexture(const Path &path, const TextureType type)
{
    int index = m_textures.size();
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
    return eastl::make_shared<MaterialHandle>(0);
}


eastl::shared_ptr<MaterialHandle> AssetManager::LoadMaterial(const Path &path, AssetData data)
{
    int index = data.index == -1 ? m_materials.size() : data.index;
    int count = data.refCount == 0 ? 1 : data.refCount;
    m_materialPaths[ToEASTLString(path.string())] = AssetData{index, count};

    // TODO: push back default material
    auto str = "I don't have that material, here is default one, "
               "i will place new material to your index later "
               + std::to_string(index);
    BF_INFO(str)

    m_materials.emplace(eastl::make_shared<Material>(GetDefaultMaterial()));
    vgjs::schedule([=]() { AddMaterialWhenLoaded(path, index); });
    return eastl::make_shared<MaterialHandle>(index);
}


void AssetManager::UpdateMaterial(const Path &relativePath)
{
    if (!HasMaterial(relativePath)) return;

    LoadMaterial(relativePath, m_materialPaths[ToEASTLString(relativePath.string())]);
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


eastl::shared_ptr<MaterialHandle> AssetManager::GetDefaultMaterialHandle()
{
    return eastl::make_shared<MaterialHandle>(0);
}


Path AssetManager::GetMaterialPath(const MaterialHandle &handle)
{
    return m_materials[handle.GetIndex()]->GetPath();
}


bool AssetManager::SceneExists(const Path &relativePath)
{
    return exists(Engine::GetContentDirectory() / relativePath);
}


void AssetManager::OpenScene(const Path &relativePath)
{
    YAML::Node scene;
    Path absolute_path(Engine::GetContentDirectory() / relativePath);

    if (exists(absolute_path))
    {
        scene = YAML::LoadFile(absolute_path.string());
    }

    Engine::ClearActiveScene();
    Engine::SetActiveScene(eastl::make_shared<Scene>(scene));
}


void AssetManager::CreateScene(const Path &relativePath)
{
    YAML::Node scene;
    Path absolute_path(Engine::GetContentDirectory() / relativePath);

    if (exists(absolute_path))
    {
        return;
    }

    scene["SceneName"] = relativePath.string();
    scene["SceneID"] = Rand::getRandomUUID().str();
    BF_DEBUG("Opening scene {0}", relativePath.filename().string());
    std::ofstream fout(absolute_path);
    fout << scene;
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


bool AssetManager::HasMaterial(const Path &relativePath)
{
    return m_materialPaths.contains(ToEASTLString(relativePath.string()));
}


void AssetManager::AddTextureWhenLoaded(const Path &path, const unsigned int index, const TextureType type)
{
    BF_INFO("Started loading texture.");
    m_textures[index] = m_loader->LoadTexture(path, type);
    auto str = "Placing texture to index " + std::to_string(index);
    BF_INFO(str);
}


void AssetManager::AddMaterialWhenLoaded(const Path &relativePath, const unsigned int index)
{
    BF_INFO("Started loading material.");
    Path absolutPat = Engine::GetContentDirectory() / relativePath;
    m_materials[index] = m_loader->LoadMaterial(absolutPat);
    auto str = "Placing material to index " + std::to_string(index);
    BF_INFO(str);
}


void AssetManager::AddMeshWhenLoaded(const Path &relativePath, const unsigned int index, const ImportMeshData data)
{
    BF_INFO("Started loading model.");
    m_meshes[index] = m_loader->ImportModel(relativePath, data);
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