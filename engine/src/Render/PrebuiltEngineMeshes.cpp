#include "Render/PrebuiltEngineMeshes.h"

namespace Blainn
{
    MeshData<> PrebuiltEngineMeshes::CreateBox(float width, float height, float depth)
    {
        MeshData<> meshData;

        BlainnVertex v[24];

        float w2 = 0.5f * width;
        float h2 = 0.5f * height;
        float d2 = 0.5f * depth;

        // Fill in the front face vertex data.
        v[0] = BlainnVertex(-w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        v[1] = BlainnVertex(-w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        v[2] = BlainnVertex(+w2, +h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        v[3] = BlainnVertex(+w2, -h2, -d2, 0.0f, 0.0f, -1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);

        // Fill in the back face vertex data.
        v[4] = BlainnVertex(-w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
        v[5] = BlainnVertex(+w2, -h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        v[6] = BlainnVertex(+w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        v[7] = BlainnVertex(-w2, +h2, +d2, 0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

        // Fill in the top face vertex data.
        v[8] = BlainnVertex(-w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        v[9] = BlainnVertex(-w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        v[10] = BlainnVertex(+w2, +h2, +d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        v[11] = BlainnVertex(+w2, +h2, -d2, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f);

        // Fill in the bottom face vertex data.
        v[12] = BlainnVertex(-w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f);
        v[13] = BlainnVertex(+w2, -h2, -d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        v[14] = BlainnVertex(+w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        v[15] = BlainnVertex(-w2, -h2, +d2, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f);

        // Fill in the left face vertex data.
        v[16] = BlainnVertex(-w2, -h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        v[17] = BlainnVertex(-w2, +h2, +d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        v[18] = BlainnVertex(-w2, +h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        v[19] = BlainnVertex(-w2, -h2, -d2, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f);

        // Fill in the right face vertex data.
        v[20] = BlainnVertex(+w2, -h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
        v[21] = BlainnVertex(+w2, +h2, -d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        v[22] = BlainnVertex(+w2, +h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f, 0.0f);
        v[23] = BlainnVertex(+w2, -h2, +d2, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,  0.0f, 0.0f, 0.0f, 1.0f, 1.0f);

        meshData.vertices.assign(&v[0], &v[24]);

        uint16_t i[36];

        // Fill in the front face index data
        i[0] = 0;
        i[1] = 1;
        i[2] = 2;
        i[3] = 0;
        i[4] = 2;
        i[5] = 3;

        // Fill in the back face index data
        i[6] = 4;
        i[7] = 5;
        i[8] = 6;
        i[9] = 4;
        i[10] = 6;
        i[11] = 7;

        // Fill in the top face index data
        i[12] = 8;
        i[13] = 9;
        i[14] = 10;
        i[15] = 8;
        i[16] = 10;
        i[17] = 11;

        // Fill in the bottom face index data
        i[18] = 12;
        i[19] = 13;
        i[20] = 14;
        i[21] = 12;
        i[22] = 14;
        i[23] = 15;

        // Fill in the left face index data
        i[24] = 16;
        i[25] = 17;
        i[26] = 18;
        i[27] = 16;
        i[28] = 18;
        i[29] = 19;

        // Fill in the right face index data
        i[30] = 20;
        i[31] = 21;
        i[32] = 22;
        i[33] = 20;
        i[34] = 22;
        i[35] = 23;

        meshData.indices.assign(&i[0], &i[36]);

        /*for (UINT i = 0; i < meshData.NumLODs; ++i)
        {
            BoundingBox::CreateFromPoints(meshData.LODBounds[i], meshData.LODVertices[i].size(),
                                          &meshData.LODVertices[i][0].position, sizeof(BlainnVertex));
        }*/

        return meshData;
    }

    MeshData<> PrebuiltEngineMeshes::CreateSphere(float radius, UINT sliceCount, UINT stackCount)
    {
        return MeshData<>();
    }

    MeshData<> PrebuiltEngineMeshes::CreateGrid(float width, float depth, UINT m, UINT n)
    {
        return MeshData<>();
    }

    MeshData<> PrebuiltEngineMeshes::CreateGeosphere(float radius, UINT numSubdivisions)
    {
        return MeshData<>();
    }

} // namespace Blainn