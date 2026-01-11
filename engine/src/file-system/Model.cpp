//
// Created by gorev on 30.09.2025.
//

#include "file-system/Model.h"
#include "Render/Device.h"
#include "Render/CommandQueue.h"

namespace Blainn
{
Model::Model()
    {
        m_meshes.reserve(4);
    }
    
    Model::Model(const Path &relativePath)
        : FileSystemObject(relativePath)
    {
        m_meshes.reserve(4);
    }

    Model::Model(const Model &other, const Path &absolutPath)
        : FileSystemObject(absolutPath)
    {
        m_meshes = other.m_meshes;
    }


    Model::Model(Model &&other, const Path &absolutPath) noexcept
        : FileSystemObject(absolutPath)
    {
        m_meshes = std::move(other.m_meshes);
    }


    Model::~Model()
    {
    }


    void Model::Copy()
    {
        FileSystemObject::Copy();
    }


    void Model::Delete()
    {
        FileSystemObject::Delete();
    }


    void Model::Move()
    {
        FileSystemObject::Move();
    }


    eastl::vector<MeshData<>> &Model::GetMeshes()
    {
        return m_meshes;
    }


    void Model::SetMeshes(const eastl::vector<MeshData<>> &meshes)
    {
        m_meshes = meshes;
    }

    void Model::CreateBufferResources()
    {
        for (auto &mesh : GetMeshes())
        {
            totalVertexCount += mesh.vertices.size();
            totalIndexCount += mesh.indices.size();
        }

        allVertices.reserve(totalVertexCount);
        allIndices.reserve(totalIndexCount);

        size_t indexValueOffsetPerMesh = 0;
        for (auto &mesh : GetMeshes())
        {
            allVertices.insert(allVertices.end(), mesh.vertices.begin(), mesh.vertices.end());
            
            auto tempCurrentMeshIndices = mesh.indices;
            eastl::for_each(tempCurrentMeshIndices.begin(), tempCurrentMeshIndices.end(), [&indexValueOffsetPerMesh](UINT& value)
                {
                    value += indexValueOffsetPerMesh;
                });

            allIndices.insert(allIndices.end(), tempCurrentMeshIndices.begin(), tempCurrentMeshIndices.end());

            indexValueOffsetPerMesh += mesh.vertices.size();
        }
    }

    void Model::CreateGPUBuffers(ID3D12GraphicsCommandList2 *pCommandList/*, UINT64 frameValue*/)
    {
        CreateGPUBuffers(pCommandList, allVertices, allIndices);
    }

    D3D12_VERTEX_BUFFER_VIEW Model::VertexBufferView() const
    {
        D3D12_VERTEX_BUFFER_VIEW vbv;
        vbv.BufferLocation = VertexBufferGPU->GetGPUVirtualAddress();
        vbv.StrideInBytes = VertexByteStride;
        vbv.SizeInBytes = VertexBufferByteSize;

        return vbv;
    }

    D3D12_INDEX_BUFFER_VIEW Model::IndexBufferView() const
    {
        D3D12_INDEX_BUFFER_VIEW ibv;
        ibv.BufferLocation = IndexBufferGPU->GetGPUVirtualAddress();
        ibv.Format = IndexFormat;
        ibv.SizeInBytes = IndexBufferByteSize;

        return ibv;
    }

    // We can free this memory after we finish upload to the GPU.
    void Model::DisposeUploaders()
    {
        VertexBufferUploader = nullptr;
        IndexBufferUploader = nullptr;
    }

} // namespace Blainn