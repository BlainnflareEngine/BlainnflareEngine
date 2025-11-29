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
    virtual ~Handle();

    unsigned int GetIndex() const;

protected:
    uuid id;
    unsigned int m_index = 0;
    AssetManager &m_manager;
};


struct TextureHandle : Handle
{
    TextureHandle(const unsigned int index, AssetManager &manager = AssetManager::GetInstance());
    virtual ~TextureHandle() override;
    Texture &GetTexture() const;
};


struct MaterialHandle : Handle
{
    MaterialHandle(const unsigned int index, AssetManager &manager = AssetManager::GetInstance());
    virtual ~MaterialHandle() override;
    Material &GetMaterial() const;
};

struct MeshHandle : Handle
{
    MeshHandle(const unsigned int index, AssetManager &manager = AssetManager::GetInstance());
    MeshHandle &operator=(const MeshHandle &other);
    virtual ~MeshHandle() override;

    Model &GetMesh() const;
};

} // namespace Blainn
