//
// Created by gorev on 01.10.2025.
//

#include "handles/Handle.h"

#include "AssetManager.h"
#include "IdGenerator.h"


Blainn::Handle::Handle(const unsigned int index)
    : id(GenerateID())
    , m_index(index)
{
}


unsigned int Blainn::Handle::GetIndex() const
{
    return m_index;
}


Blainn::Texture &Blainn::TextureHandle::GetTexture() const
{
    return AssetManager::GetInstance().GetTextureByIndex(m_index);
}


Blainn::Material &Blainn::MaterialHandle::GetMaterial() const
{
    return AssetManager::GetInstance().GetMaterialByIndex(m_index);
}