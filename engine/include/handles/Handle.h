//
// Created by gorev on 30.09.2025.
//

#pragma once


#include "aliases.h"


#include <pch.h>

namespace Blainn
{
class Material;
class Texture;
class Model;

struct Handle
{
    Handle(unsigned int index);

    unsigned int GetIndex() const;

protected:
    uuid id;
    unsigned int m_index = 0;
};


struct TextureHandle : Handle
{
    Texture &GetTexture() const;
};


struct MaterialHandle : Handle
{
    Material &GetMaterial() const;
};

struct ModelHandle : Handle
{
    Model &GetModel() const;
};

} // namespace Blainn
