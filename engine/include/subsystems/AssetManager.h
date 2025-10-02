//
// Created by gorev on 30.09.2025.
//

#pragma once
#include "AssetLoader.h"
#include "EASTL/set.h"

namespace Blainn
{

struct MaterialHandle;
struct TextureHandle;
struct Model;


class AssetManager
{
public:
    static AssetManager &GetInstance();

    void Init();
    void Destroy();

    // Returns pointer to model, loads model additionally if not exists
    eastl::shared_ptr<Model> GetModel(const std::filesystem::path &path);

    // Add material by
    // eastl::shared_ptr<MaterialHandle> AddMaterial(const std::filesystem::path &path);

    // Returns pointer to texture, loads texture additionally if not exists
    // eastl::shared_ptr<TextureHandle> GetTextureHandle(const std::filesystem::path &path);

    eastl::shared_ptr<TextureHandle> GetTexture(const std::filesystem::path &path);
    Texture &GetTextureByIndex(unsigned int index);
    Texture &GetTextureByHandle(const TextureHandle &handle);

private:
    AssetManager() = default;
    AssetManager(const AssetManager &) = delete;
    AssetManager &operator=(const AssetManager &) = delete;
    AssetManager(const AssetManager &&) = delete;
    AssetManager &operator=(const AssetManager &&) = delete;

    void AddTextureWhenLoaded(const Path& path, int index);

private:
    inline static eastl::unique_ptr<AssetLoader> m_loader;

    eastl::hash_map<eastl::string, eastl::shared_ptr<Model>> m_models;
    eastl::set<eastl::shared_ptr<MaterialHandle>> m_materials;

    // eastl::vector<eastl::shared_ptr<TextureHandle>> m_textureHandles;
    eastl::hash_map<eastl::string, int> m_texture_paths;
    eastl::vector<Texture> m_textures;

    eastl::unique_ptr<Texture> m_defaultTexture;
};

} // namespace Blainn