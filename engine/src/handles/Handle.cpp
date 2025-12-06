//
// Created by gorev on 01.10.2025.
//

#include "handles/Handle.h"

#include "AssetManager.h"
#include "random.h"


Blainn::Handle::Handle(const unsigned int index, AssetManager &manager)
    : id(Rand::getRandomUUID())
    , m_index(index)
    , m_manager(manager)
{
}


Blainn::Handle::~Handle()
{
}


unsigned int Blainn::Handle::GetIndex() const
{
    return m_index;
}


Blainn::TextureHandle::TextureHandle(const unsigned int index, AssetManager &manager)
    : Handle(index, manager)
{
    manager.IncreaseTextureRefCount(index);
}


Blainn::TextureHandle::~TextureHandle()
{
    m_manager.DecreaseTextureRefCount(m_index);
}


Blainn::Texture &Blainn::TextureHandle::GetTexture() const
{
    return AssetManager::GetInstance().GetTextureByIndex(m_index);
}


Blainn::MaterialHandle::MaterialHandle(const unsigned int index, AssetManager &manager)
    : Handle(index, manager)
{
    manager.IncreaseMaterialRefCount(index);
}


Blainn::MaterialHandle::~MaterialHandle()
{
    m_manager.DecreaseMaterialRefCount(m_index);
}


Blainn::Material &Blainn::MaterialHandle::GetMaterial() const
{
    return AssetManager::GetInstance().GetMaterialByIndex(m_index);
}


Blainn::MeshHandle::MeshHandle(const unsigned int index, AssetManager &manager)
    : Handle(index, manager)
{
    manager.IncreaseMeshRefCount(index);
}

Blainn::MeshHandle::~MeshHandle()
{
    m_manager.DecreaseMeshRefCount(m_index);
}


Blainn::Model &Blainn::MeshHandle::GetMesh() const
{
    return AssetManager::GetInstance().GetMeshByIndex(m_index);
}