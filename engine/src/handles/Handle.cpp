//
// Created by gorev on 01.10.2025.
//

#include "handles/Handle.h"

#include "AssetManager.h"
#include "IdGenerator.h"


Blainn::Handle::Handle()
{
    id = GenerateID();
}


Blainn::TextureHandle::TextureHandle(unsigned int index)
    : Handle()
    , m_textureIndex(index)
{
}


Blainn::Texture &Blainn::TextureHandle::GetTexture() const
{
    return AssetManager::GetInstance().GetTextureByIndex(m_textureIndex);
}


unsigned int Blainn::TextureHandle::GetTextureIndex() const
{
    return m_textureIndex;
}


Blainn::MaterialHandle::MaterialHandle()
    : Handle()
{
}