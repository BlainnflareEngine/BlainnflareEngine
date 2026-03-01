#pragma once

#include "Render/DXHelpers.h"
#include "MeshData.h"

namespace Blainn
{
    enum class EPrebuiltMeshType : uint32_t
    {
        BOX = 0u, // default
        SPHERE,    // point light mesh
        CONE,      // spot light mesh
        GRID,      // optional for plane representing or world grid
        GEOSPHERE, // optional for point light and other
        NUM_PREBUILT_MESHES
    };

    class PrebuiltEngineMeshes
    {
    public:
        struct BoxParams
        {
            float width = 1.0f;
            float height = 1.0f;
            float depth = 1.0f;
        };

        struct SphereParams
        {
            float radius = 1.0f;
            UINT sliceCount = 16u;
            UINT stackCount = 16u;
        };

        struct CylinderParams
        {
            float baseRadius = 1.0f;
            float topRadius = 0.0f;
            float height = 1.0f;
            UINT sectorCount = 16u;
        };

        struct GridParams
        {
            float width = 1.0f;
            float depth = 1.0f;
            UINT rows = 2u;
            UINT columns = 2u;
        };

        struct GeosphereParams
        {
            float radius = 1.0f;
            UINT numSubdivisions = 1u;
        };

        static MeshData<> CreateBox(const BoxParams &params);

        static MeshData<> CreateSphere(const SphereParams &params);

        // Cone & Cylinder
        static MeshData<> CreateCylinder(const CylinderParams &params);

        static MeshData<> CreateGrid(const GridParams &params);

        static MeshData<> CreateGeosphere(const GeosphereParams &params);

    private:
        static void Subdivide(MeshData<> &meshData);
        static BlainnVertex MidPoint(const BlainnVertex &v0, const BlainnVertex &v1);
    };
}
