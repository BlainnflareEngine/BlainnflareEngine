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
    Handle();

    uuid id;
};


struct TextureHandle : Handle
{
    TextureHandle(unsigned int index);


    Texture &GetTexture() const;


    unsigned int GetTextureIndex() const;

private:
    unsigned int m_textureIndex = 0;
};


struct MaterialHandle : Handle
{
    MaterialHandle();

    eastl::shared_ptr<Material> material;
};

} // namespace Blainn
