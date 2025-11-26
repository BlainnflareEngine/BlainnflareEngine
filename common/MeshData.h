//
// Created by gorev on 30.09.2025.
//

#pragma once

#include "handles/Handle.h"
#include "VertexTypes.h"

#include <pch.h>

namespace Blainn
{
//template<typename TVertex = Vertex, typename TIndex = uint32_t>
struct MeshData
{
    //static_assert(std::is_same<TIndex, unsigned>() || std::is_same<TIndex, unsigned short>());
    struct MaterialHandle;

    MeshData(uint8_t numLODs = 1u)
        : NumLODs(numLODs)
    {
        vertices.reserve(256);
        indices.reserve(256);
    }


    MeshData(const eastl::vector<BlainnVertex>& vertices, const eastl::vector<UINT>& indices, const eastl::shared_ptr<MaterialHandle> &materialHandle, uint8_t numLODs = 1u)
        : vertices(vertices)
        , indices(indices)
        , materialHandle(materialHandle)
        , NumLODs(numLODs)
    {

    }


    eastl::vector<BlainnVertex> vertices;
    eastl::vector<UINT> indices;
    Mat4 parentMatrix;
    eastl::shared_ptr<MaterialHandle> materialHandle;
    uint8_t NumLODs; 
};
} // namespace Blainn
