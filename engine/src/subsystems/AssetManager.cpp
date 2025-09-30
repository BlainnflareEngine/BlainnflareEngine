//
// Created by gorev on 30.09.2025.
//

#include "subsystems/AssetManager.h"

#include "file-system/Model.h"

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
}


void AssetManager::Destroy()
{
    BF_INFO("AssetManager Destroy");
    m_loader->Destroy();
}


Model &AssetManager::GetModel(const std::filesystem::path &path)
{
    eastl::string pathStr = path.string().c_str();
    if (auto it = m_models.find(pathStr); it != m_models.end())
    {
        return *it->second;
    }

    Model model = m_loader->ImportModel(path);
    m_models[pathStr] = eastl::make_unique<Model>(model);
    return *m_models[pathStr];
}
} // namespace Blainn