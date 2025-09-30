//
// Created by gorev on 30.09.2025.
//

#pragma once

#include <pch.h>


namespace Blainn
{
class Material;
class Texture;

struct Handle
{
    Handle()
    {
        UUIDv4::UUIDGenerator<std::mt19937_64> uuidGenerator;
        id = uuidGenerator.getUUID();
    }


    uuid id;
};


struct TextureHandle : Handle
{
    TextureHandle()
        : Handle()
    {
    }

    eastl::shared_ptr<Texture> texture;
};


struct MaterialHandle : Handle
{
    MaterialHandle()
        : Handle()
    {
    }

    eastl::shared_ptr<Material> material;
};

} // namespace Blainn
