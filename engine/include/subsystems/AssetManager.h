//
// Created by gorev on 30.09.2025.
//

#pragma once
#include "AssetLoader.h"
#include "Model.h"

namespace Blainn
{

class AssetManager
{
public:
    static AssetManager &GetInstance();

    static void Init();
    static void Destroy();

    Model &GetModel(const std::filesystem::path &path);

private:
    AssetManager() = default;
    AssetManager(const AssetManager &) = delete;
    AssetManager &operator=(const AssetManager &) = delete;
    AssetManager(const AssetManager &&) = delete;
    AssetManager &operator=(const AssetManager &&) = delete;

private:
    inline static eastl::unique_ptr<AssetLoader> m_loader;

    eastl::hash_map<eastl::string, eastl::unique_ptr<Model>> m_models;
};

} // namespace Blainn