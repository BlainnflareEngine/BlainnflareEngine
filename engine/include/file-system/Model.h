//
// Created by gorev on 30.09.2025.
//

#pragma once
#include "FileSystemObject.h"
#include "MeshData.h"
#include "Render/FreyaUtil.h"
#include "Render/Device.h"

#include <EASTL/vector.h>

namespace Blainn
{
class Device;

class Model : public FileSystemObject
{
    friend class AssetLoader;

public:
    Model();
    Model(const Path &relativePath);
    Model(const Model &other, const Path &absolutPath);
    Model(Model &&other, const Path &absolutPath) noexcept;
    ~Model() override;

    virtual void Copy() override;
    virtual void Delete() override;
    virtual void Move() override;

    eastl::vector<MeshData<>> &GetMeshes();
    void SetMeshes(const eastl::vector<MeshData<>> &meshes);

public:
    void CreateBufferResources();
    size_t GetVerticesCount() const
    {
        return totalVertexCount;
    }
    size_t GetIndicesCount() const
    {
        return totalIndexCount;
    }

    void CreateGPUBuffers(ID3D12GraphicsCommandList2 *pCommandList /*, uint32_t64 frameValue*/);
    void DisposeUploaders();

    // TO DO: proper way: texture transoform matrix for every MeshData object
    const Mat4 &GetTextureTransform() const
    {
        return m_texTransform;
    }
    void SetTextureTransform(const Mat4 &texTranform)
    {
        m_texTransform = texTranform;
    }

    bool IsLoaded() const
    {
        return m_bisLoaded;
    }

private:
    template <typename TVertex, typename TIndex = uint32_t>
    void CreateGPUBuffers(ID3D12GraphicsCommandList2 *pCommandList, const eastl::vector<TVertex> &vertices,
                          const eastl::vector<TIndex> &indices = eastl::vector<TIndex>(0))
    {
        static_assert(eastl::is_same<TIndex, unsigned>() || eastl::is_same<TIndex, unsigned short>());

        auto &device = Device::GetInstance();

        // TODO
        const uint64_t vbByteSize = vertices.size() * sizeof(TVertex);
        const uint32_t ibByteSize = (uint32_t)indices.size() * sizeof(TIndex);

        if (vbByteSize)
        {
            VertexBufferGPU = FreyaUtil::CreateDefaultBuffer(device.GetDevice2().Get(), pCommandList, vertices.data(),
                                                             vbByteSize, VertexBufferUploader); // Create GPU resource
            // For vertex buffer view.
            VertexBufferByteSize = (uint32_t)vbByteSize;
            VertexByteStride = sizeof(TVertex);
        }

        if (ibByteSize)
        {
            IndexBufferGPU = FreyaUtil::CreateDefaultBuffer(device.GetDevice2().Get(), pCommandList, indices.data(),
                                                            ibByteSize, IndexBufferUploader); // Create GPU resource
            // For index buffer view.
            IndexBufferByteSize = ibByteSize;
            IndexFormat = (eastl::is_same<TIndex, unsigned short>()) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
        }
    }

#pragma region VertexIndexBuffersViewStuff
public:
    // uint64_t GetModelFrameValue() const { return m_frameValue; }
    D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;
    D3D12_INDEX_BUFFER_VIEW IndexBufferView() const;

    eastl::vector<BlainnVertex> &GetAllVertices()
    {
        return allVertices;
    }

    eastl::vector<uint32_t> &GetAllIndices()
    {
        return allIndices;
    }

private:
    Mat4 m_texTransform = Mat4::Identity;

    // Data about the buffers.
    uint32_t VertexByteStride = 0u;
    uint32_t VertexBufferByteSize = 0u;
    DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
    uint32_t IndexBufferByteSize = 0u;

#pragma endregion VertexIndexBuffersViewStuff
    eastl::vector<BlainnVertex> allVertices;
    eastl::vector<uint32_t> allIndices;
    size_t totalVertexCount = 0u;
    size_t totalIndexCount = 0u;

    // the actual default buffer resource
    Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferGPU = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferGPU = nullptr;

    // an intermediate upload heap in order to copy CPU memory data into out default buffer
    Microsoft::WRL::ComPtr<ID3D12Resource> VertexBufferUploader = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> IndexBufferUploader = nullptr;

private:
    eastl::vector<MeshData<>> m_meshes;

    bool m_bisLoaded = false;
};

} // namespace Blainn