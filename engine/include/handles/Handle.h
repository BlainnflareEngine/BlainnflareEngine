//
// Created by gorev on 30.09.2025.
//

#pragma once

#include "file-system/Material.h"
#include "file-system/Texture.h"


#include <pch.h>

namespace Blainn
{

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

} // namespace Blainn
