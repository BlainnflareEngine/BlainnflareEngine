//
// Created by gorev on 01.10.2025.
//

#include "handles/Handle.h"

#include "AssetManager.h"
#include "random.h"
#include "file-system/Material.h"

using namespace Blainn;

Handle::Handle(const unsigned int index, AssetManager &manager)
    : id(Rand::getRandomUUID())
    , m_index(index)
    , m_manager(manager)
{
}

Handle::~Handle()
{
}

unsigned int Handle::GetIndex() const
{
    return m_index;
}

TextureHandle::TextureHandle(const unsigned int index, AssetManager &manager)
    : Handle(index, manager)
{
    manager.IncreaseTextureRefCount(index);
}

TextureHandle::~TextureHandle()
{
    m_manager.DecreaseTextureRefCount(m_index);
}

Blainn::Texture &TextureHandle::GetTexture() const
{
    auto& texture = AssetManager::GetInstance().GetTextureByIndex(m_index);
    return texture.IsLoaded() ? texture : AssetManager::GetInstance().GetTextureByIndex(0);
}

unsigned int TextureHandle::GetIndex() const
{
    return AssetManager::GetInstance().GetTextureByIndex(m_index).IsLoaded() ? m_index : 0; 
}

MaterialHandle::MaterialHandle(const unsigned int index, AssetManager &manager)
    : Handle(index, manager)
{
    manager.IncreaseMaterialRefCount(index);
}

MaterialHandle::~MaterialHandle()
{
    m_manager.DecreaseMaterialRefCount(m_index);
}

Material &MaterialHandle::GetMaterial() const
{
    if (AssetManager::GetInstance().GetMaterialByIndex(m_index).AreTexturesLoaded())
        return AssetManager::GetInstance().GetMaterialByIndex(m_index);
    return AssetManager::GetInstance().GetMaterialByIndex(0);
}

unsigned int MaterialHandle::GetIndex() const
{
    if (AssetManager::GetInstance().GetMaterialByIndex(m_index).AreTexturesLoaded())
        return m_index;
    return 0;
}

MeshHandle::MeshHandle(const unsigned int index, AssetManager &manager)
    : Handle(index, manager)
{
    manager.IncreaseMeshRefCount(index);
}

MeshHandle::~MeshHandle()
{
    m_manager.DecreaseMeshRefCount(m_index);
}

Model &MeshHandle::GetMesh() const
{
    return AssetManager::GetInstance().GetMeshByIndex(m_index);
}