#include "Render/PrebuiltEngineMeshes.h"

namespace Blainn
{
    MeshData PrebuiltEngineMeshes::CreateBox(float width, float height, float depth)
    {
        MeshData meshData;

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
    MeshData PrebuiltEngineMeshes::ÑreateGrid(float width, float depth, UINT m, UINT n)
    {
        MeshData meshData;

        UINT vertexCount = m * n;
        UINT faceCount = (m - 1) * (n - 1) * 2;

        //
        // Create the vertices.
        //

        float halfWidth = 0.5f * width;
        float halfDepth = 0.5f * depth;

        float dx = width / (n - 1);
        float dz = depth / (m - 1);

        float du = 1.0f / (n - 1);
        float dv = 1.0f / (m - 1);

        meshData.vertices.resize(vertexCount);
        for (UINT i = 0; i < m; ++i)
        {
            float z = halfDepth - i * dz;
            for (UINT j = 0; j < n; ++j)
            {
                float x = -halfWidth + j * dx;

                meshData.vertices[i * n + j].position = XMFLOAT3(x, 0.0f, z);
                meshData.vertices[i * n + j].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
                meshData.vertices[i * n + j].tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);

                // Stretch texture over grid.
                meshData.vertices[i * n + j].texCoord.x = j * du;
                meshData.vertices[i * n + j].texCoord.y = i * dv;
            }
        }

        //
        // Create the indices.
        //

        meshData.vertices.resize(faceCount * 3); // 3 indices per face

        // Iterate over each quad and compute indices.
        UINT k = 0;
        for (UINT i = 0; i < m - 1; ++i)
        {
            for (UINT j = 0; j < n - 1; ++j)
            {
                meshData.indices[k] = i * n + j;
                meshData.indices[k + 1] = i * n + j + 1;
                meshData.indices[k + 2] = (i + 1) * n + j;

                meshData.indices[k + 3] = (i + 1) * n + j;
                meshData.indices[k + 4] = i * n + j + 1;
                meshData.indices[k + 5] = (i + 1) * n + j + 1;

                k += 6; // next quad
            }
        }

        return meshData;
    }
} // namespace Blainn