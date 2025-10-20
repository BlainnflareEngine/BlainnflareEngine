//
// Created by gorev on 30.09.2025.
//

#pragma once
#include <pch.h>

namespace Blainn
{
struct Vertex
{
    Vertex(){};

    Vertex(const Vec3 &position, const Vec3 &normal, const Vec3 &tangent, const Vec3 &bitangent, const Vec2 &texCoord)
        : position(position)
        , normal(normal)
        , tangent(tangent)
        , bitangent(bitangent)
        , texCoord(texCoord)
    {
    }

    Vec3 position;
    Vec3 normal;
    Vec3 tangent;
    Vec3 bitangent;
    Vec2 texCoord;
};
} // namespace Blainn
