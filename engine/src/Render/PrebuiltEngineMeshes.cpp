#include "Render/PrebuiltEngineMeshes.h"

namespace Blainn
{
    MeshData<> PrebuiltEngineMeshes::CreateBox(const BoxParams &params)
    {
        MeshData<> meshData;

        BlainnVertex v[24];

        float w2 = 0.5f * params.width;
        float h2 = 0.5f * params.height;
        float d2 = 0.5f * params.depth;

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

    MeshData<> PrebuiltEngineMeshes::CreateSphere(const SphereParams &params)
    {
        MeshData<> meshData;

        BlainnVertex topVertex(0.0f, +params.radius, 0.0f, 0.0f, +1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                               0.0f, 0.0f);
        BlainnVertex bottomVertex(0.0f, -params.radius, 0.0f, 0.0f, -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                  0.0f, 1.0f);

        meshData.vertices.push_back(topVertex);

        float phiStep = XM_PI / static_cast<float>(params.stackCount);
        float thetaStep = 2.0f * XM_PI / static_cast<float>(params.sliceCount);

        // Compute vertices for each stack ring (do not count the poles as rings).
        for (UINT i = 1; i <= params.stackCount - 1; ++i)
        {
            float phi = static_cast<float>(i) * phiStep;

            // Vertices of ring.
            for (UINT j = 0; j <= params.sliceCount; ++j)
            {
                float theta = static_cast<float>(j) * thetaStep;

                BlainnVertex v;

                // spherical to cartesian
                v.position.x = params.radius * sinf(phi) * cosf(theta);
                v.position.y = params.radius * cosf(phi);
                v.position.z = params.radius * sinf(phi) * sinf(theta);

                // Partial derivative of P with respect to theta
                v.tangent.x = -params.radius * sinf(phi) * sinf(theta);
                v.tangent.y = 0.0f;
                v.tangent.z = +params.radius * sinf(phi) * cosf(theta);

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

        for (UINT i = 1; i <= params.sliceCount; ++i)
        {
            meshData.indices.push_back(0);
            meshData.indices.push_back(i + 1);
            meshData.indices.push_back(i);
        }

        // Offset the indices to the index of the first vertex in the first ring.
        // This is just skipping the top pole vertex.
        UINT baseIndex = 1;
        UINT ringVertexCount = params.sliceCount + 1;
        for (UINT i = 0; i < params.stackCount - 2; ++i)
        {
            for (UINT j = 0; j < params.sliceCount; ++j)
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

        for (UINT i = 0; i < params.sliceCount; ++i)
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

    MeshData<> PrebuiltEngineMeshes::CreateCylinder(const CylinderParams &params)
    {
        MeshData<> meshData;

        float x, y, z; // vertex position

        const float PI = std::acos(-1.0f);
        float sectorStep = 2.0f * PI / static_cast<float>(params.sectorCount);
        float sectorAngle; // radian

        // compute the normal vector at 0 degree first
        // tanA = (baseRadius-topRadius) / height
        float zAngle = std::atan2(params.baseRadius - params.topRadius, params.height);
        float x0 = std::cos(zAngle); // nx
        float y0 = 0.0f;             // ny
        float z0 = std::sin(zAngle); // nz

        // rotate (x0,y0,z0) per sector angle
        std::vector<float> sideNormals;
        for (UINT i = 0; i <= params.sectorCount; ++i)
        {
            sectorAngle = static_cast<float>(i) * sectorStep;
            sideNormals.push_back(std::cos(sectorAngle) * x0 - std::sin(sectorAngle) * y0); // nx
            sideNormals.push_back(std::sin(sectorAngle) * x0 + std::cos(sectorAngle) * y0); // ny
            sideNormals.push_back(z0);                                            // nz
        }

        std::vector<float> unitCircleVertices;
        for (UINT i = 0; i <= params.sectorCount; ++i)
        {
            sectorAngle = static_cast<float>(i) * sectorStep;
            unitCircleVertices.push_back(std::cos(sectorAngle)); // x
            unitCircleVertices.push_back(std::sin(sectorAngle)); // y
            unitCircleVertices.push_back(0.0f);                  // z
        }

        // remember where the base.top vertices start
        unsigned int baseVertexIndex = (unsigned int)meshData.vertices.size();
        // TODO: Convert this
        // put vertices of base of cylinder
        z = -params.height * 0.5f;
        
        meshData.vertices.push_back(BlainnVertex(0.0f, 0.0f, z, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f));

        for (UINT i = 0, j = 0; i < params.sectorCount; ++i, j += 3)
        {
            x = unitCircleVertices[j];
            y = unitCircleVertices[j + 1];
            meshData.vertices.push_back(
                BlainnVertex(x * params.baseRadius, y * params.baseRadius, z, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, 0.0f, -x * 0.5f + 0.5f, -y * 0.5f + 0.5f));
        }

        // remember where the top vertices start
        unsigned int topVertexIndex = (unsigned int)meshData.vertices.size();
        // put vertices of top of cylinder
        z = params.height * 0.5f;
        meshData.vertices.push_back(BlainnVertex(0.0f, 0.0f, z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.5f));
        
        for (UINT i = 0, j = 0; i < params.sectorCount; ++i, j += 3)
        {
            x = unitCircleVertices[j];
            y = unitCircleVertices[j + 1];
            meshData.vertices.push_back(
                BlainnVertex(x * params.topRadius, y * params.topRadius, z, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                             0.0f, 0.0f, x * 0.5f + 0.5f, -y * 0.5f + 0.5f));
        }

        UINT k1 = 0u; // 1st vertex index at base
        UINT k2 = params.sectorCount + 1u; // 1st vertex index at top

        for (UINT j = 0; j < params.sectorCount; ++j, ++k1, ++k2)
        {
            if (j == 0)
            {
                // 2 trianles per sector
                meshData.indices.push_back(k1 + params.sectorCount);
                meshData.indices.push_back(k1 + 1);
                meshData.indices.push_back(k2 + params.sectorCount);

                meshData.indices.push_back(k2 + params.sectorCount);
                meshData.indices.push_back(k1 + 1);
                meshData.indices.push_back(k2 + 1);
            }
            else
            {
                // 2 trianles per sector
                meshData.indices.push_back(k1);
                meshData.indices.push_back(k1 + 1);
                meshData.indices.push_back(k2);

                meshData.indices.push_back(k2);
                meshData.indices.push_back(k1 + 1);
                meshData.indices.push_back(k2 + 1);
            }
        }

        // put indices for base
        for (UINT i = 0, k = baseVertexIndex + 1u; i < params.sectorCount; ++i, ++k)
        {
            if (i < (params.sectorCount - 1u))
            {
                meshData.indices.push_back(baseVertexIndex);
                meshData.indices.push_back(k + 1);
                meshData.indices.push_back(k);
            }
            else // last triangle
            {
                meshData.indices.push_back(baseVertexIndex);
                meshData.indices.push_back(baseVertexIndex + 1);
                meshData.indices.push_back(k);
            }
        }

        // put indices for top
        for (UINT i = 0, k = topVertexIndex + 1u; i < params.sectorCount; ++i, ++k)
        {
            if (i < (params.sectorCount - 1u))
            {
                meshData.indices.push_back(topVertexIndex);
                meshData.indices.push_back(k);
                meshData.indices.push_back(k + 1);
            }
            else
            {
                meshData.indices.push_back(topVertexIndex);
                meshData.indices.push_back(k);
                meshData.indices.push_back(topVertexIndex + 1);
            }
        }

        return meshData;
    }

    MeshData<> PrebuiltEngineMeshes::CreateGrid(const GridParams &params)
    {
        MeshData<> meshData;

        UINT vertexCount = params.rows * params.columns;
        UINT faceCount = (params.rows - 1u) * (params.columns - 1u) * 2u;

        //
        // Create the vertices.
        //

        float halfWidth = 0.5f * params.width;
        float halfDepth = 0.5f * params.depth;

        float dx = params.width / static_cast<float>(params.columns - 1u);
        float dz = params.depth / static_cast<float>(params.rows - 1u);

        float du = 1.0f / static_cast<float>(params.columns - 1u);
        float dv = 1.0f / static_cast<float>(params.rows - 1u);

        meshData.vertices.resize(vertexCount);
        for (UINT i = 0; i < params.rows; ++i)
        {
            float z = halfDepth - static_cast<float>(i) * dz;
            for (UINT j = 0; j < params.columns; ++j)
            {
                float x = -halfWidth + static_cast<float>(j) * dx;

                meshData.vertices[i * params.columns + j].position = XMFLOAT3(x, 0.0f, z);
                meshData.vertices[i * params.columns + j].normal = XMFLOAT3(0.0f, 1.0f, 0.0f);
                meshData.vertices[i * params.columns + j].tangent = XMFLOAT3(1.0f, 0.0f, 0.0f);

                // Stretch texture over grid.
                meshData.vertices[i * params.columns + j].texCoord.x = static_cast<float>(j) * du;
                meshData.vertices[i * params.columns + j].texCoord.y = static_cast<float>(i) * dv;
            }
        }

        //
        // Create the indices.
        //

        meshData.indices.resize(static_cast<size_t>(faceCount) * 3u); // 3 indices per face

        // Iterate over each quad and compute indices.
        UINT k = 0;
        for (UINT i = 0; i < params.rows - 1u; ++i)
        {
            for (UINT j = 0; j < params.columns - 1u; ++j)
            {
                meshData.indices[k] = i * params.columns + j;
                meshData.indices[k + 1] = i * params.columns + j + 1u;
                meshData.indices[k + 2] = (i + 1u) * params.columns + j;

                meshData.indices[k + 3] = (i + 1u) * params.columns + j;
                meshData.indices[k + 4] = i * params.columns + j + 1u;
                meshData.indices[k + 5] = (i + 1u) * params.columns + j + 1u;

                k += 6; // next quad
            }
        }

        return meshData;
    }

    MeshData<> PrebuiltEngineMeshes::CreateGeosphere(const GeosphereParams &params)
    {
        MeshData<> meshData;

        UINT numSubdivisions = std::min<UINT>(params.numSubdivisions, 6u);

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
            XMVECTOR p = params.radius * n;

            XMStoreFloat3(&meshData.vertices[i].position, p);
            XMStoreFloat3(&meshData.vertices[i].normal, n);

            // Derive texture coordinates from spherical coordinates.
            float theta = atan2f(meshData.vertices[i].position.z, meshData.vertices[i].position.x);

            // Put in [0, 2pi].
            if (theta < 0.0f) theta += XM_2PI;

            float phi = acosf(meshData.vertices[i].position.y / params.radius);

            meshData.vertices[i].texCoord.x = theta / XM_2PI;
            meshData.vertices[i].texCoord.y = phi / XM_PI;

            // Partial derivative of P with respect to theta
            meshData.vertices[i].tangent.x = -params.radius * sinf(phi) * sinf(theta);
            meshData.vertices[i].tangent.y = 0.0f;
            meshData.vertices[i].tangent.z = +params.radius * sinf(phi) * cosf(theta);

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
