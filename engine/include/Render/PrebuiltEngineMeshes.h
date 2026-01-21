#pragma once

#include "Render/DXHelpers.h"
#include "MeshData.h"

namespace Blainn
{
    class PrebuiltEngineMeshes
    {
    public:
        static MeshData<> CreateBox(float width, float height, float depth);

        static MeshData<> CreateSphere(float radius, UINT sliceCount, UINT stackCount);

        static MeshData<> CreateGrid(float width, float depth, UINT m, UINT n);

        static MeshData<> CreateGeosphere(float radius, UINT numSubdivisions);
    };
}