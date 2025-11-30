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
    public:
        Model();
        Model(const Path &absolutPath);
        Model(const Model &other, const Path &absolutPath);
        Model(Model &&other, const Path &absolutPath) noexcept;
        ~Model() override;

        virtual void Copy() override;
        virtual void Delete() override;
        virtual void Move() override;

        eastl::vector<MeshData<>> &GetMeshes();
        void SetMeshes(const eastl::vector<MeshData<>> &meshes);

    public:
        void CreateBufferResources(ID3D12GraphicsCommandList2* pCommandList = nullptr);
        
        template <typename TVertex, typename TIndex = UINT>
        void CreateGPUBuffers(ID3D12GraphicsCommandList2* pCommandList, const eastl::vector<TVertex> &vertices, const eastl::vector<TIndex> &indices = eastl::vector<TIndex>(0))
        {
            static_assert(eastl::is_same<TIndex, unsigned>() || eastl::is_same<TIndex, unsigned short>());

            auto& device = Device::GetInstance();

            // TODO
            const UINT64 vbByteSize = vertices.size() * sizeof(TVertex);
            const UINT ibByteSize = (UINT)indices.size() * sizeof(TIndex);

            if (vbByteSize)
            {
                VertexBufferGPU = FreyaUtil::CreateDefaultBuffer(device.GetDevice2().Get(), pCommandList,
                                                                 vertices.data(), vbByteSize,
                                                   VertexBufferUploader); // Create GPU resource
                // For vertex buffer view.
                VertexBufferByteSize = (UINT)vbByteSize;
                VertexByteStride = sizeof(TVertex);
            }

            if (ibByteSize)
            {
                IndexBufferGPU = FreyaUtil::CreateDefaultBuffer(device.GetDevice2().Get(), pCommandList, indices.data(), ibByteSize, IndexBufferUploader); // Create GPU resource
                // For index buffer view.
                IndexBufferByteSize = ibByteSize;
                IndexFormat = (eastl::is_same<TIndex, unsigned short>()) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT;
            }
        }

        void DisposeUploaders();

        size_t GetVerticesCount() const { return totalVertexCount; }
        size_t GetIndicesCount() const { return totalIndexCount; }

#pragma region VertexIndexBuffersViewStuff
        D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const;
        D3D12_INDEX_BUFFER_VIEW IndexBufferView() const;

    private:
        // Data about the buffers.
        UINT VertexByteStride = 0u;
        UINT VertexBufferByteSize = 0u;
        DXGI_FORMAT IndexFormat = DXGI_FORMAT_R16_UINT;
        UINT IndexBufferByteSize = 0u;
#pragma endregion VertexIndexBuffersViewStuff

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
    };

} // namespace Blainn