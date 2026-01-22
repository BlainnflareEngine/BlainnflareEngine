#pragma once

#include "Render/DXHelpers.h"
#include "MeshData.h"

namespace Blainn
{
    enum EPrebuiltMeshType : uint8_t
    {
        CUBE = 0u,
        SPHERE,
        GRID,
        GEOSPHERE,
        NUM_PREBUILT_MESHES
    };

    class PrebuiltEngineMeshes
    {
    public:
        static MeshData<> CreateBox(float width, float height, float depth);

        static MeshData<> CreateSphere(float radius, UINT sliceCount, UINT stackCount);

        static MeshData<> CreateGrid(float width, float depth, UINT m, UINT n);

        static MeshData<> CreateGeosphere(float radius, UINT numSubdivisions);

    private:
        static void Subdivide(MeshData<> &meshData);
        static BlainnVertex MidPoint(const BlainnVertex &v0, const BlainnVertex &v1);
    };
}