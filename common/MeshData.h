//
// Created by gorev on 30.09.2025.
//

#pragma once

#include <pch.h>

#include "VertexTypes.h"
#include "Handles/Handle.h"

namespace Blainn
{
template<typename TVertex = BlainnVertex, typename TIndex = uint32_t>
struct MeshData
{
    static_assert(std::is_same<TIndex, unsigned>() || std::is_same<TIndex, unsigned short>());
    //struct MaterialHandle;

    MeshData(uint8_t numLODs = 1u)
        : NumLODs(numLODs)
    {
        vertices.reserve(256);
        indices.reserve(256);
    }

    MeshData(const eastl::vector<TVertex>& vertices, const eastl::vector<TIndex>& indices, const eastl::shared_ptr<MaterialHandle> &materialHandle, uint8_t numLODs = 1u)
        : vertices(vertices)
        , indices(indices)
        //, materialHandle(materialHandle)
        , NumLODs(numLODs)
    {

    }


    eastl::vector<TVertex> vertices;
    eastl::vector<TIndex> indices;
    Mat4 parentMatrix;
    //eastl::shared_ptr<MaterialHandle> materialHandle;
    uint8_t NumLODs; 
};
} // namespace Blainn
