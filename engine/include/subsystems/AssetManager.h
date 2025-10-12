//
// Created by gorev on 30.09.2025.
//

#pragma once
#include "AssetLoader.h"

namespace Blainn
{
struct MaterialHandle;
class TextureHandle;
struct ModelHandle;


class AssetManager
{
public:
    static AssetManager &GetInstance();

    void Init();
    void Destroy();

    bool ModelExists(const Path &path);
    eastl::shared_ptr<ModelHandle> GetModel(const Path &path);
    eastl::shared_ptr<ModelHandle> LoadModel(const Path &path, const ImportModelData &data);
    Model &GetModelByIndex(unsigned int index);
    Model &GetModelByHandle(const ModelHandle &handle);

    bool HasTexture(const Path &path);
    eastl::shared_ptr<TextureHandle> GetTexture(const Path &path);
    eastl::shared_ptr<TextureHandle> LoadTexture(const Path &path, const Texture::TextureType type);
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

    void AddTextureWhenLoaded(const Path &path, const unsigned int index, const Texture::TextureType type);
    void AddMaterialWhenLoaded(const Path &path, const unsigned int index);
    void AddModelWhenLoaded(const Path &path, const unsigned int index, const ImportModelData data);

    Texture &GetDefaultTexture();
    Material &GetDefaultMaterial();
    Model &GetDefaultModel();

private:
    inline static eastl::unique_ptr<AssetLoader> m_loader;

    // eastl::hash_map<eastl::string, eastl::shared_ptr<Model>> m_models;

    // TODO: use vector with free list
    eastl::hash_map<eastl::string, unsigned int> m_materialPaths;
    eastl::vector<eastl::shared_ptr<Material>> m_materials;

    // TODO: use vector with free list
    eastl::hash_map<eastl::string, unsigned int> m_texturePaths;
    eastl::vector<eastl::shared_ptr<Texture>> m_textures;

    eastl::hash_map<eastl::string, unsigned int> m_meshPaths;
    eastl::vector<eastl::shared_ptr<Model>> m_meshes;
};

} // namespace Blainn