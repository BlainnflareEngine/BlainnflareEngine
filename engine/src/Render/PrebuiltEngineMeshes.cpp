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
        MeshData<> meshData;

        BlainnVertex topVertex(0.0f, +radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
        BlainnVertex bottomVertex(0.0f, -radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);

        meshData.vertices.push_back(topVertex);

        float phiStep = XM_PI / stackCount;
        float thetaStep = 2.0f * XM_PI / sliceCount;

        // Compute vertices for each stack ring (do not count the poles as rings).
        for (UINT i = 1; i <= stackCount - 1; ++i)
        {
            float phi = i * phiStep;

            // Vertices of ring.
            for (UINT j = 0; j <= sliceCount; ++j)
            {
                float theta = j * thetaStep;

                BlainnVertex v;

                // spherical to cartesian
                v.position.x = radius * sinf(phi) * cosf(theta);
                v.position.y = radius * cosf(phi);
                v.position.z = radius * sinf(phi) * sinf(theta);

                // Partial derivative of P with respect to theta
                v.tangent.x = -radius * sinf(phi) * sinf(theta);
                v.tangent.y = 0.0f;
                v.tangent.z = +radius * sinf(phi) * cosf(theta);

                XMVECTOR T = XMLoadFloat3(&v.tangent);
                XMStoreFloat3(&v.tangent, XMVector3Normalize(T));

                XMVECTOR p = XMLoadFloat3(&v.position);
                XMStoreFloat3(&v.normal, XMVector3Normalize(p));

                v.texCoord.x = theta / XM_2PI;
                v.texCoord.y = phi / XM_PI;

                meshData.vertices.push_back(v);
            }
        }

        meshData.vertices.push_back(bottomVertex);

        for (UINT i = 1; i <= sliceCount; ++i)
        {
            meshData.indices.push_back(0);
            meshData.indices.push_back(i + 1);
            meshData.indices.push_back(i);
        }

        // Offset the indices to the index of the first vertex in the first ring.
        // This is just skipping the top pole vertex.
        UINT baseIndex = 1;
        UINT ringVertexCount = sliceCount + 1;
        for (UINT i = 0; i < stackCount - 2; ++i)
        {
            for (UINT j = 0; j < sliceCount; ++j)
            {
                meshData.indices.push_back(baseIndex + i * ringVertexCount + j);
                meshData.indices.push_back(baseIndex + i * ringVertexCount + j + 1);
                meshData.indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);

                meshData.indices.push_back(baseIndex + (i + 1) * ringVertexCount + j);
                meshData.indices.push_back(baseIndex + i * ringVertexCount + j + 1);
                meshData.indices.push_back(baseIndex + (i + 1) * ringVertexCount + j + 1);
            }
        }

        // South pole vertex was added last.
        UINT southPoleIndex = (UINT)meshData.vertices.size() - 1;

        // Offset the indices to the index of the first vertex in the last ring.
        baseIndex = southPoleIndex - ringVertexCount;

        for (UINT i = 0; i < sliceCount; ++i)
        {
            meshData.indices.push_back(southPoleIndex);
            meshData.indices.push_back(baseIndex + i);
            meshData.indices.push_back(baseIndex + i + 1);
        }

        /*for (UINT i = 0; i < meshData.NumLODs; ++i)
        {
            BoundingBox::CreateFromPoints(meshData.LODBounds[i], meshData.LODVertices[i].size(),
                                          &meshData.LODVertices[i][0].position, sizeof(VertexPositionNormalTangentUV));
        }*/

        return meshData;
    }

    MeshData<> PrebuiltEngineMeshes::CreateGrid(float width, float depth, UINT m, UINT n)
    {
        MeshData<> meshData;

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

        meshData.indices.resize(faceCount * 3); // 3 indices per face

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

    MeshData<> PrebuiltEngineMeshes::CreateGeosphere(float radius, UINT numSubdivisions)
    {
        MeshData<> meshData;

        numSubdivisions = std::min<UINT>(numSubdivisions, 6u);

        // Approximate a sphere by tessellating an icosahedron.
        const float X = 0.525731f;
        const float Z = 0.850651f;

        XMFLOAT3 pos[12] = {XMFLOAT3(-X, 0.0f, Z), XMFLOAT3(X, 0.0f, Z),   XMFLOAT3(-X, 0.0f, -Z),
                            XMFLOAT3(X, 0.0f, -Z), XMFLOAT3(0.0f, Z, X),   XMFLOAT3(0.0f, Z, -X),
                            XMFLOAT3(0.0f, -Z, X), XMFLOAT3(0.0f, -Z, -X), XMFLOAT3(Z, X, 0.0f),
                            XMFLOAT3(-Z, X, 0.0f), XMFLOAT3(Z, -X, 0.0f),  XMFLOAT3(-Z, -X, 0.0f)};

        UINT k[60] = {1, 4,  0, 4, 9, 0,  4, 5, 9,  8, 5, 4,  1,  8,  4, 1, 10, 8,  10, 3,
                      8, 8,  3, 5, 3, 2,  5, 3, 7,  2, 3, 10, 7,  10, 6, 7, 6,  11, 7,  6,
                      0, 11, 6, 1, 0, 10, 1, 6, 11, 0, 9, 2,  11, 9,  5, 2, 9,  11, 2,  7};

        meshData.vertices.resize(12);
        meshData.indices.assign(&k[0], &k[60]);

        for (UINT i = 0; i < 12; ++i)
            meshData.vertices[i].position = pos[i];

        for (UINT i = 0; i < numSubdivisions; ++i)
            Subdivide(meshData);

        // Project vertices onto sphere and scale.
        for (UINT i = 0; i < meshData.vertices.size(); ++i)
        {
            // Project onto unit sphere.
            XMVECTOR n = XMVector3Normalize(XMLoadFloat3(&meshData.vertices[i].position));

            // Project onto sphere.
            XMVECTOR p = radius * n;

            XMStoreFloat3(&meshData.vertices[i].position, p);
            XMStoreFloat3(&meshData.vertices[i].normal, n);

            // Derive texture coordinates from spherical coordinates.
            float theta = atan2f(meshData.vertices[i].position.z, meshData.vertices[i].position.x);

            // Put in [0, 2pi].
            if (theta < 0.0f) theta += XM_2PI;

            float phi = acosf(meshData.vertices[i].position.y / radius);

            meshData.vertices[i].texCoord.x = theta / XM_2PI;
            meshData.vertices[i].texCoord.y = phi / XM_PI;

            // Partial derivative of P with respect to theta
            meshData.vertices[i].tangent.x = -radius * sinf(phi) * sinf(theta);
            meshData.vertices[i].tangent.y = 0.0f;
            meshData.vertices[i].tangent.z = +radius * sinf(phi) * cosf(theta);

            XMVECTOR T = XMLoadFloat3(&meshData.vertices[i].tangent);
            XMStoreFloat3(&meshData.vertices[i].tangent, XMVector3Normalize(T));
        }

        return meshData;
    }

    void PrebuiltEngineMeshes::Subdivide(MeshData<> &meshData)
    {
        // Save a copy of the input geometry.
        MeshData<> inputCopy = meshData;

        meshData.vertices.resize(0);
        meshData.vertices.resize(0);

        //       v1
        //       *
        //      / \
	//     /   \
	//  m0*-----*m1
        //   / \   / \
	//  /   \ /   \
	// *-----*-----*
        // v0    m2     v2

        UINT numTris = (UINT)inputCopy.indices.size() / 3;
        for (UINT i = 0; i < numTris; ++i)
        {
            BlainnVertex v0 = inputCopy.vertices[inputCopy.indices[i * 3 + 0]];
            BlainnVertex v1 = inputCopy.vertices[inputCopy.indices[i * 3 + 1]];
            BlainnVertex v2 = inputCopy.vertices[inputCopy.indices[i * 3 + 2]];

            //
            // Generate the midpoints.
            //

            BlainnVertex m0 = MidPoint(v0, v1);
            BlainnVertex m1 = MidPoint(v1, v2);
            BlainnVertex m2 = MidPoint(v0, v2);

            //
            // Add new geometry.
            //

            meshData.vertices.push_back(v0); // 0
            meshData.vertices.push_back(v1); // 1
            meshData.vertices.push_back(v2); // 2
            meshData.vertices.push_back(m0); // 3
            meshData.vertices.push_back(m1); // 4
            meshData.vertices.push_back(m2); // 5

            meshData.indices.push_back(i * 6 + 0);
            meshData.indices.push_back(i * 6 + 3);
            meshData.indices.push_back(i * 6 + 5);

            meshData.indices.push_back(i * 6 + 3);
            meshData.indices.push_back(i * 6 + 4);
            meshData.indices.push_back(i * 6 + 5);

            meshData.indices.push_back(i * 6 + 5);
            meshData.indices.push_back(i * 6 + 4);
            meshData.indices.push_back(i * 6 + 2);

            meshData.indices.push_back(i * 6 + 3);
            meshData.indices.push_back(i * 6 + 1);
            meshData.indices.push_back(i * 6 + 4);
        }
    }

    BlainnVertex PrebuiltEngineMeshes::MidPoint(const BlainnVertex &v0, const BlainnVertex &v1)
    {
        XMVECTOR p0 = XMLoadFloat3(&v0.position);
        XMVECTOR p1 = XMLoadFloat3(&v1.position);

        XMVECTOR n0 = XMLoadFloat3(&v0.normal);
        XMVECTOR n1 = XMLoadFloat3(&v1.normal);

        XMVECTOR tan0 = XMLoadFloat3(&v0.tangent);
        XMVECTOR tan1 = XMLoadFloat3(&v1.tangent);

        XMVECTOR tex0 = XMLoadFloat2(&v0.texCoord);
        XMVECTOR tex1 = XMLoadFloat2(&v1.texCoord);

        // Compute the midpoints of all the attributes.  Vectors need to be normalized
        // since linear interpolating can make them not unit length.
        XMVECTOR pos = 0.5f * (p0 + p1);
        XMVECTOR normal = XMVector3Normalize(0.5f * (n0 + n1));
        XMVECTOR tangent = XMVector3Normalize(0.5f * (tan0 + tan1));
        XMVECTOR tex = 0.5f * (tex0 + tex1);

        BlainnVertex v;
        XMStoreFloat3(&v.position, pos);
        XMStoreFloat3(&v.normal, normal);
        XMStoreFloat3(&v.tangent, tangent);
        XMStoreFloat2(&v.texCoord, tex);

        return v;
    }

} // namespace Blainn