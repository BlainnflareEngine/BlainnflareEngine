#pragma once

#include "Render/DXHelpers.h"
#include "MeshData.h"

namespace Blainn
{
    enum class EPrebuiltMeshType : uint32_t
    {
        CUBE = 0u, // default
        SPHERE,    // point light mesh
        CONE,      // spot light mesh
        GEOSPHERE, // optional for point light and other
        GRID,      // optional for plane representing
        NUM_PREBUILT_MESHES
    };

    class PrebuiltEngineMeshes
    {
    public:
        static MeshData<> CreateBox(float width, float height, float depth);

        static MeshData<> CreateSphere(float radius, UINT sliceCount, UINT stackCount);

        // Cone & Cylinder
        static MeshData<> CreateCylinder(int baseRadius, int topRadius, int height, int sectorCount);

        static MeshData<> CreateGrid(float width, float depth, UINT m, UINT n);

        static MeshData<> CreateGeosphere(float radius, UINT numSubdivisions);

    private:
        static void Subdivide(MeshData<> &meshData);
        static BlainnVertex MidPoint(const BlainnVertex &v0, const BlainnVertex &v1);
    };
}