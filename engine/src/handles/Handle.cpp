//
// Created by gorev on 01.10.2025.
//

#include "handles/Handle.h"

#include "AssetManager.h"


Blainn::Handle::Handle()
{
    UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
    id = uuidGenerator.getUUID();
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