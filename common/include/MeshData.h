//
// Created by gorev on 30.09.2025.
//

#pragma once

#include "Vertex.h"
#include "handles/Handle.h"

#include <pch.h>

namespace Blainn
{
struct MeshData
{
    struct MaterialHandle;

    MeshData()
    {
        vertices.reserve(256);
        indices.reserve(256);
    }


    MeshData(const eastl::vector<Vertex> &vertices, const eastl::vector<unsigned int> &indices,
             const eastl::shared_ptr<MaterialHandle> &materialHandle)
        : vertices(vertices)
        , indices(indices)
        , materialHandle(materialHandle)
    {
    }


    eastl::vector<Vertex> vertices;
    eastl::vector<unsigned int> indices;
    Mat4 parentMatrix;
    eastl::shared_ptr<MaterialHandle> materialHandle;
};
} // namespace Blainn
