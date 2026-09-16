#pragma once

#include "Render/DXHelpers.h"

namespace Blainn
{
    struct FGBufferTexture
    {
        ComPtr<ID3D12Resource> m_resource = nullptr;
        CD3DX12_CPU_DESCRIPTOR_HANDLE m_hCpuRtvDsv = {};
        CD3DX12_CPU_DESCRIPTOR_HANDLE m_hCpuSrv = {};
        CD3DX12_GPU_DESCRIPTOR_HANDLE m_hGpuSrv = {};
    };

    class GBuffer final
    {
    public:
        enum EGBufferLayer : UINT
        {
            DIFFUSE_ALBEDO = 0u,
            AMBIENT_OCCLUSION,
            NORMAL,
            SPECULAR,
            DEPTH, // must be the last texture

            MAX = 5u
        };

    public:
        GBuffer(ID3D12Device *device, UINT width, UINT height);
        GBuffer(const GBuffer &buffer) = delete;
        GBuffer &operator=(const GBuffer &buffer) = delete;

        ~GBuffer() noexcept;

    public:
        FORCEINLINE UINT GetWidth() const
        {
            return m_width;
        }
        FORCEINLINE UINT GetHeight() const
        {
            return m_height;
        }

        // if screen resized
        void OnResize(UINT newWidth, UINT newHeight);

        ID3D12Resource *Get(const unsigned layer) const;
        FGBufferTexture *GetBufferTexture(const unsigned layer);
        DXGI_FORMAT GetBufferTextureFormat(const unsigned layer) const;

        CD3DX12_GPU_DESCRIPTOR_HANDLE GetSrv(const unsigned layer) const;
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtv(const unsigned layer) const;
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsv(const unsigned layer) const;

        void SetDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv, CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsvRtv, const unsigned layer);
        void CreateDescriptors();

    private:
        void CreateResources();

    private:
        ID3D12Device *m_device = nullptr;
        UINT m_width, m_height;
        FGBufferTexture m_buffer[EGBufferLayer::MAX];
    };
}