﻿//
// Created by gorev on 30.09.2025.
//

#pragma once
#include "AssetLoader.h"

namespace Blainn
{
struct MaterialHandle;
struct TextureHandle;
struct MeshHandle;


class AssetManager
{
    struct AssetData
    {
        unsigned int index;
        unsigned int refCount;
    };

public:
    static AssetManager &GetInstance();

    void Init();
    void Destroy();

    bool MeshExists(const Path &path);
    eastl::shared_ptr<MeshHandle> GetMesh(const Path &path);
    eastl::shared_ptr<MeshHandle> LoadMesh(const Path &path, const ImportModelData &data);
    Model &GetMeshByIndex(unsigned int index);
    Model &GetMeshByHandle(const MeshHandle &handle);

    bool HasTexture(const Path &path);
    eastl::shared_ptr<TextureHandle> GetTexture(const Path &path);
    eastl::shared_ptr<TextureHandle> LoadTexture(const Path &path, const TextureType type);
    Texture &GetTextureByIndex(unsigned int index);
    Texture &GetTextureByHandle(const TextureHandle &handle);

    bool HasMaterial(const Path &path);
    eastl::shared_ptr<MaterialHandle> GetMaterial(const Path &path);
    eastl::shared_ptr<MaterialHandle> LoadMaterial(const Path &path);
    Material &GetMaterialByIndex(unsigned int index);
    Material &GetMaterialByHandle(const MaterialHandle &handle);


private:
    AssetManager() = default;
    AssetManager(const AssetManager &) = delete;
    AssetManager &operator=(const AssetManager &) = delete;
    AssetManager(const AssetManager &&) = delete;
    AssetManager &operator=(const AssetManager &&) = delete;

    friend struct MeshHandle;
    friend struct MaterialHandle;
    friend struct TextureHandle;
    friend struct Handle;


    void AddTextureWhenLoaded(const Path &path, const unsigned int index, const TextureType type);
    void AddMaterialWhenLoaded(const Path &path, const unsigned int index);
    void AddModelWhenLoaded(const Path &path, const unsigned int index, const ImportModelData data);

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

    // TODO: use vector with free list
    eastl::hash_map<eastl::string, AssetData> m_materialPaths;
    eastl::vector<eastl::shared_ptr<Material>> m_materials;

    // TODO: use vector with free list
    eastl::hash_map<eastl::string, AssetData> m_texturePaths;
    eastl::vector<eastl::shared_ptr<Texture>> m_textures;

    eastl::hash_map<eastl::string, AssetData> m_meshPaths;
    eastl::vector<eastl::shared_ptr<Model>> m_meshes;
};

} // namespace Blainn