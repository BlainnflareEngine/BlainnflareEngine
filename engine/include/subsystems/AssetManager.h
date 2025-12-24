//
// Created by gorev on 30.09.2025.
//

#pragma once
#include "AssetLoader.h"
#include "FreeListVector.h"
#include "helpers.h"


namespace Blainn
{
struct MaterialHandle;
struct TextureHandle;
struct MeshHandle;
struct ImportMeshData;

#ifndef MAX_TEXTURES
    #define MAX_TEXTURES 512 // 128 for diff, normal, metallic and roughness (skip AO for now)
#endif
#ifndef MAX_MATERIALS
    #define MAX_MATERIALS 64
#endif
#ifndef MAX_MESHES
    #define MAX_MESHES 64
#endif

class AssetManager
{
    friend class RenderSubsystem;

    struct AssetData
    {
        int index;
        int refCount;
    };

public:
    NO_COPY_NO_MOVE(AssetManager);

    static AssetManager &GetInstance();

    void Init();
    void Destroy();

    bool HasMesh(const Path &relativePath);
    eastl::shared_ptr<MeshHandle> GetMesh(const Path &relativePath);
    static eastl::shared_ptr<MeshHandle> GetDefaultMesh();
    eastl::shared_ptr<MeshHandle> LoadMesh(const Path &relativePath, const ImportMeshData &data);
    Model &GetMeshByIndex(unsigned int index);
    Model &GetMeshByHandle(const MeshHandle &handle);
    Path GetMeshPath(const MeshHandle &handle);

    bool HasTexture(const Path &path);
    eastl::shared_ptr<TextureHandle> GetTexture(const Path &path);
    eastl::shared_ptr<TextureHandle> LoadTexture(const Path &relativePath, const TextureType type);
    Texture &GetTextureByIndex(unsigned int index);
    Texture &GetTextureByHandle(const TextureHandle &handle);

    bool HasMaterial(const Path &relativePath);
    eastl::shared_ptr<MaterialHandle> GetMaterial(const Path &path);
    eastl::shared_ptr<MaterialHandle> LoadMaterial(const Path &path, AssetData data = {-1, 0});
    void UpdateMaterial(const Path &relativePath);
    Material &GetMaterialByIndex(unsigned int index);
    Material &GetMaterialByHandle(const MaterialHandle &handle);
    static eastl::shared_ptr<MaterialHandle> GetDefaultMaterialHandle();
    Path GetMaterialPath(const MaterialHandle &handle);

    void ResetTextures();
    static bool SceneExists(const Path &relativePath);
    static void OpenScene(const Path &relativePath);
    static void CreateScene(const Path &relativePath);

private:
    AssetManager() = default;

    friend struct MeshHandle;
    friend struct MaterialHandle;
    friend struct TextureHandle;
    friend struct Handle;

    void AddTextureWhenLoaded(const Path &path, const unsigned int index, const TextureType type);
    void AddMaterialWhenLoaded(const Path &relativePath, const unsigned int index);
    void AddMeshWhenLoaded(const Path &relativePath, const unsigned int index, const ImportMeshData data);

    Texture &GetDefaultTexture();
    Material &GetDefaultMaterial();
    Model &GetDefaultModel();

    void IncreaseTextureRefCount(const unsigned int index);
    void IncreaseMaterialRefCount(const unsigned int index);
    void IncreaseMeshRefCount(const unsigned int index);
    void DecreaseTextureRefCount(const unsigned int index);
    void DecreaseMaterialRefCount(const unsigned int index);
    void DecreaseMeshRefCount(const unsigned int index);

private:
    inline static eastl::unique_ptr<AssetLoader> m_loader;

    eastl::hash_map<eastl::string, AssetData> m_materialPaths;
    FreeListVector<eastl::shared_ptr<Material>> m_materials;

    eastl::hash_map<eastl::string, AssetData> m_texturePaths;
    FreeListVector<eastl::shared_ptr<Texture>> m_textures;

    eastl::hash_map<eastl::string, AssetData> m_meshPaths;
    FreeListVector<eastl::shared_ptr<Model>> m_meshes;
};

} // namespace Blainn