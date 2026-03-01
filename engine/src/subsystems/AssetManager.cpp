//
// Created by gorev on 30.09.2025.
//

#include "Subsystems/AssetManager.h"

#include "Engine.h"
#include "ImportAssetData.h"
#pragma warning(push)
#pragma warning(disable : 4100)
#include "VGJS.h"
#pragma warning(pop)
#include "File-System/Material.h"
#include "File-System/Model.h"
#include "File-System/Texture.h"

#include "Render/PrebuiltEngineMeshes.h"

#include "Navigation/NavigationSubsystem.h"

#include "scene/SceneManager.h"
#include "scene/SceneManagerTemplates.h"
#include "scene/Scene.h"

namespace Blainn
{
const Path relativeDefaultDiffuseTexturePath = "Textures/Default.dds";
const Path relativeDefaultMaterialPath = "Materials/Default.mat";

AssetManager &AssetManager::GetInstance()
{
    static AssetManager instance;
    return instance;
}


void AssetManager::Init()
{
    BF_INFO("AssetManager Init");

    // For DirectXTex
    ThrowIfFailed(CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED));

    m_loader = eastl::make_unique<AssetLoader>();
    m_loader->Init();

    m_textures.reserve(MAX_TEXTURES);
    m_materials.reserve(MAX_MATERIALS);
    m_meshes.reserve(MAX_MESHES);

    LoadDefaultTextures();
    LoadDefaultMaterials();
    LoadPrebuiltMeshes();
}

void AssetManager::LoadDefaultTextures()
{
    m_textures.emplace(m_loader->LoadTexture(relativeDefaultDiffuseTexturePath, TextureType::ALBEDO, 0u));
    m_texturePaths[ToEASTLString(relativeDefaultDiffuseTexturePath.string())] = {0, 1};
}

void AssetManager::LoadDefaultMaterials()
{
    Material material = Material(relativeDefaultMaterialPath, "");
    material.SetTexture(GetTexture(relativeDefaultDiffuseTexturePath), TextureType::ALBEDO);
    
    m_materialPaths[ToEASTLString(relativeDefaultMaterialPath.string())] = {0, 1};
    m_materials.emplace(eastl::make_shared<Material>(eastl::move(material)));
}

void AssetManager::LoadPrebuiltMeshes()
{
    // could be rewritten with a lambda i suppose
    Model model;
#pragma region Box
    model = Model{};
    model.SetMeshes({PrebuiltEngineMeshes::CreateBox(1.f, 1.f, 1.f)});
    model.CreateBufferResources();
    model.CreateGPUBuffers();
    m_meshes.emplace(eastl::make_shared<Model>(model));
#pragma endregion Box
#pragma region Sphere
    model = Model{};
    model.SetMeshes({PrebuiltEngineMeshes::CreateSphere(1.0f, 16u, 16u)});
    model.CreateBufferResources();
    model.CreateGPUBuffers();
    m_meshes.emplace(eastl::make_shared<Model>(model));
#pragma endregion Sphere
#pragma region Cone
    model = Model{};
    model.SetMeshes({PrebuiltEngineMeshes::CreateCylinder(1, 0, 1, 16)});
    model.CreateBufferResources();
    model.CreateGPUBuffers();
    m_meshes.emplace(eastl::make_shared<Model>(model));
#pragma endregion Cone
#pragma region Grid
    model = Model{};
    model.SetMeshes({PrebuiltEngineMeshes::CreateGrid(50.0f, 50.0f, 20u, 20u)});
    model.CreateBufferResources();
    model.CreateGPUBuffers();
    m_meshes.emplace(eastl::make_shared<Model>(model));
#pragma endregion Grid
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

    return GetDefaultMesh();
}


eastl::shared_ptr<MeshHandle> AssetManager::GetDefaultMesh(uint32_t index /* = 0u*/)
{
    return eastl::make_shared<MeshHandle>(index);
}


eastl::shared_ptr<MeshHandle> AssetManager::LoadMesh(const Path &relativePath, const ImportMeshData &data)
{
    int index = static_cast<int>(m_meshes.emplace(eastl::make_shared<Model>(GetDefaultModel(), relativePath)));
    m_meshPaths[ToEASTLString(relativePath.string())] = AssetData{index, 1};
    vgjs::schedule([=]() { AddMeshWhenLoaded(relativePath, index, data); });

    return eastl::make_shared<MeshHandle>(index);
}


Model &AssetManager::GetMeshByIndex(const unsigned int index)
{
    if (index >= m_meshes.size()) return GetDefaultMesh()->GetMesh();

    if (m_meshes[index]->IsLoaded()) return *m_meshes[index];

    return GetDefaultMesh()->GetMesh();
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
    assert(path.is_relative());
    return m_texturePaths.find(ToEASTLString(path.string())) != m_texturePaths.end();
}


eastl::shared_ptr<TextureHandle> AssetManager::GetTexture(const Path &path)
{
    if (auto it = m_texturePaths.find(ToEASTLString(path.string())); it != m_texturePaths.end())
        return eastl::make_shared<TextureHandle>(it->second.index);

    BF_ERROR("Texture doesn't exist. You should load it first. Texture - {0}", path.string());
    return eastl::make_shared<TextureHandle>(0);
}


eastl::shared_ptr<TextureHandle> AssetManager::LoadTexture(const Path &relativePath, const TextureType type)
{
    assert(relativePath.is_relative());

    int index = static_cast<int>(m_textures.push_back(eastl::make_shared<Texture>()));
    m_texturePaths[ToEASTLString(relativePath.string())] = AssetData{index, 1};

    vgjs::schedule([=]() { AddTextureWhenLoaded(relativePath, index, type); });
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
    assert(path.is_relative() && "the path is not relative");

    int index = data.index == -1 ? static_cast<int>(m_materials.emplace(eastl::make_shared<Material>())) : data.index;
    int count = data.refCount == 0 ? 1 : data.refCount;
    m_materialPaths[ToEASTLString(path.string())] = AssetData{index, count};
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
    if (index >= m_materials.size()) return GetDefaultMaterial();

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
    auto a = m_materials[handle.GetIndex()]->GetPath();
    return m_materials[handle.GetIndex()]->GetPath();
}


bool AssetManager::SceneExists(const Path &relativePath)
{
    return exists(Engine::GetContentDirectory() / relativePath);
}


void AssetManager::OpenScene(const Path &relativePath)
{
    NavigationSubsystem::ClearNavMesh();

    YAML::Node scene;
    Path absolute_path(Engine::GetContentDirectory() / relativePath);

    if (exists(absolute_path))
    {
        scene = YAML::LoadFile(absolute_path.string());
    }

    Engine::GetSceneManager().CloseScenes();
    Engine::GetSceneManager().OpenScene(scene, Single);
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
    std::ofstream fout(absolute_path);
    fout << scene;
}


Texture &AssetManager::GetTextureByIndex(unsigned int index)
{
    return *m_textures[index];
}


Texture &AssetManager::GetTextureByHandle(const TextureHandle &handle)
{
    assert(false);
    unsigned int index = handle.GetIndex();
    if (index >= m_textures.size())
    {
        BF_FATAL("Failed to get texture by index. Index is greater than m_textures size!");
        throw std::exception("Failed to get texture by index.");
    }

    return *m_textures[index];
}

Path AssetManager::GetTexturePath(const TextureHandle &handle)
{
    return m_textures[handle.GetIndex()]->GetPath();
}

bool AssetManager::HasMaterial(const Path &relativePath)
{
    return m_materialPaths.contains(ToEASTLString(relativePath.string()));
}


void AssetManager::ResetTextures()
{
    m_loader->ResetTextureOffsetsTable();
}

void AssetManager::AddTextureWhenLoaded(const Path &path, const unsigned int index, const TextureType type)
{
    assert(path.is_relative());

    m_textures[index] = m_loader->LoadTexture(path, type, index);
}


void AssetManager::AddMaterialWhenLoaded(const Path &relativePath, const unsigned int index)
{
    assert(relativePath.is_relative());
    m_materials[index] = m_loader->LoadMaterial(relativePath);
}


void AssetManager::AddMeshWhenLoaded(const Path &relativePath, const unsigned int index, const ImportMeshData data)
{
    assert(relativePath.is_relative());
    m_meshes[index] = m_loader->ImportModel(relativePath, data);
}


Texture &AssetManager::GetDefaultTexture(uint32_t index /*= 0u*/)
{
    return *m_textures[index];
}


Material &AssetManager::GetDefaultMaterial()
{
    return *m_materials[0];
}


Model &AssetManager::GetDefaultModel(uint32_t index /*= 0u*/)
{
    return *m_meshes[index];
}


void AssetManager::IncreaseTextureRefCount(const unsigned int index)
{
    for (auto &[key, value] : m_texturePaths)
        if (static_cast<unsigned int>(value.index) == index) ++value.refCount;
}


void AssetManager::IncreaseMaterialRefCount(const unsigned int index)
{
    for (auto &[key, value] : m_materialPaths)
        if (static_cast<unsigned int>(value.index) == index) ++value.refCount;
}


void AssetManager::IncreaseMeshRefCount(const unsigned int index)
{
    for (auto &[key, value] : m_meshPaths)
        if (static_cast<unsigned int>(value.index) == index) ++value.refCount;
}


void AssetManager::DecreaseTextureRefCount(const unsigned int index)
{
    // 0: default diffuse
    if (index == 0) return;

    for (auto &[key, value] : m_texturePaths)
    {
        if (static_cast<unsigned int>(value.index) == index)
        {
            --value.refCount;

            if (value.refCount == 1) // 1 because we have shared ptr in free list vector
            {
                Device::GetInstance().Flush();
                m_textures.erase(index);
                m_texturePaths.erase(key);
                return;
            }
        }
    }
}


void AssetManager::DecreaseMaterialRefCount(const unsigned int index)
{
    if (index == 0) return;

    for (auto &[key, value] : m_materialPaths)
    {
        if (static_cast<unsigned int>(value.index) == index)
        {
            --value.refCount;

            if (value.refCount == 1) // 1 because we have shared ptr in free list vector
            {
                m_materials.erase(index);
                m_materialPaths.erase(key);
                return;
            }
        }
    }
}


void AssetManager::DecreaseMeshRefCount(const unsigned int index)
{
    if (index == 0) return;

    for (auto &[key, value] : m_meshPaths)
    {
        if (static_cast<unsigned int>(value.index) == index)
        {
            --value.refCount;


            if (value.refCount == 1) // 1 because we have shared ptr in free list vector
            {
                Device::GetInstance().Flush();
                m_meshes.erase(index);
                m_meshPaths.erase(key);
                return;
            }
        }
    }
}

} // namespace Blainn
