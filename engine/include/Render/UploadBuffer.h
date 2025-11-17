#pragma once

#include "Render/FreyaUtil.h"

using namespace Microsoft::WRL;

namespace Blainn{

    template<typename T>
    class UploadBuffer
    {
        public:
        UploadBuffer(ID3D12Device* device, uint32_t elementCount, bool isConstantBuffer)
		:
		m_isConstantBuffer(isConstantBuffer)
        {
            m_elementByteSize = sizeof(T);

            if (isConstantBuffer) m_elementByteSize = FreyaUtil::CalcConstantBufferByteSize(sizeof(T));
            
            auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
            auto buffer = CD3DX12_RESOURCE_DESC::Buffer(m_elementByteSize * elementCount);

            ThrowIfFailed(device->CreateCommittedResource(
                &uploadHeap,
                D3D12_HEAP_FLAG_NONE,
			    &buffer,
			    D3D12_RESOURCE_STATE_GENERIC_READ,
			    nullptr,
			    IID_PPV_ARGS(&m_uploadBuffer)));

            ThrowIfFailed(m_uploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedData)));
        }

        UploadBuffer(const UploadBuffer& rhs) = delete;
        UploadBuffer& operator=(const UploadBuffer& rhs) = delete;
        
        ~UploadBuffer()
        {
            if (m_uploadBuffer)
            {
                m_uploadBuffer->Unmap(0, nullptr);
            }
        }
        
        ID3D12Resource* Get() const
        {
            return m_uploadBuffer.Get();
        }
        
        void CopyData(int elementIndex, const T& data)
        {
            memcpy(&m_mappedData[elementIndex * m_elementByteSize], &data, sizeof(T));
        }
        
        private:
        ComPtr<ID3D12Resource> m_uploadBuffer; // either constant or vertex/index buffer
        BYTE* m_mappedData = nullptr;
        uint32_t m_elementByteSize = 0u;
        bool m_isConstantBuffer = false;
    };
}