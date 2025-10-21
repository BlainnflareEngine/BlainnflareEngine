//
// Created by gorev on 30.09.2025.
//

#pragma once


#include "AssetManager.h"
#include "aliases.h"


namespace Blainn
{
class Material;
class Texture;
class Model;

struct Handle
{
    Handle(const unsigned int index, AssetManager &manager = AssetManager::GetInstance());

    unsigned int GetIndex() const;

protected:
    uuid id;
    unsigned int m_index = 0;
};


struct TextureHandle : Handle
{
    TextureHandle(const unsigned int index, AssetManager &manager = AssetManager::GetInstance());
    Texture &GetTexture() const;
};


struct MaterialHandle : Handle
{
    MaterialHandle(const unsigned int index, AssetManager &manager = AssetManager::GetInstance());
    Material &GetMaterial() const;
};

struct MeshHandle : Handle
{
    MeshHandle(const unsigned int index, AssetManager &manager = AssetManager::GetInstance());
    Model &GetMesh() const;
};

} // namespace Blainn
