#pragma once

#include "Render/DXHelpers.h"
#include "MeshData.h"

namespace Blainn
{
    class PrebuiltEngineMeshes
    {
    public:
        static MeshData CreateBox(float width, float height, float depth);

        static MeshData ÑreateGrid(float width, float depth, UINT m, UINT n);
    };
}