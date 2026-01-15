//
// Created by gorev on 30.09.2025.
//

#pragma once

#include "aliases.h"
#include "AssetManager.h"

namespace Blainn
{
class Material;
class Texture;
class Model;

struct Handle
{
    Handle(const unsigned int index, AssetManager &manager = AssetManager::GetInstance());
    virtual ~Handle();

    virtual unsigned int GetIndex() const;

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

    unsigned int GetIndex() const override;
};


struct MaterialHandle : Handle
{
    MaterialHandle(const unsigned int index, AssetManager &manager = AssetManager::GetInstance());
    virtual ~MaterialHandle() override;
    Material &GetMaterial() const;

    virtual unsigned int GetIndex() const override;
};

struct MeshHandle : Handle
{
    MeshHandle(const unsigned int index, AssetManager &manager = AssetManager::GetInstance());
    virtual ~MeshHandle() override;

    Model &GetMesh() const;
};

} // namespace Blainn
