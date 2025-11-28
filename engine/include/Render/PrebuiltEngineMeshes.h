#pragma once

#include "Render/DXHelpers.h"
#include "MeshData.h"

namespace Blainn
{
    class PrebuiltEngineMeshes
    {
    public:
        static MeshData CreateBox(float width, float height, float depth);
    };
}