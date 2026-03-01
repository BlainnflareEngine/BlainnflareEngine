#pragma once

#include "Render/DXHelpers.h"

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

        MAX = 5
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

    ID3D12Resource *Get(unsigned layer);
    FGBufferTexture *GetBufferTexture(unsigned layer);
    DXGI_FORMAT GetBufferTextureFormat(unsigned layer);

    CD3DX12_GPU_DESCRIPTOR_HANDLE GetSrv(unsigned layer) const;
    CD3DX12_CPU_DESCRIPTOR_HANDLE GetRtv(unsigned layer) const;
    CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsv(unsigned layer) const;

    void SetDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
                        CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsvRtv, unsigned layer);
    void CreateDescriptors();

private:
    void CreateResources();

private:
    ID3D12Device *m_device = nullptr;

    UINT m_width, m_height;

    FGBufferTexture m_buffer[EGBufferLayer::MAX];
    // maybe better decision to create a map [EGBufferLayer, DXGI_FORMAT]
    static constexpr DXGI_FORMAT
        m_bufferFormats[EGBufferLayer::MAX] = // order of DXGI_FORMAT should corresponds to EGBufferLayer
        {
            DXGI_FORMAT_R8G8B8A8_UNORM,     // DIFFUSE_ALBEDO
            DXGI_FORMAT_R8G8B8A8_UNORM,     // AMBIENT_OCCLUSION
            DXGI_FORMAT_R32G32B32A32_FLOAT, // NORMAL
            DXGI_FORMAT_R8G8B8A8_UNORM,     // SPECULAR
            DXGI_FORMAT_D24_UNORM_S8_UINT   // DEPTH. Format for DSV (SRV demands R24...)
    };

    static constexpr FLOAT m_optimizedClearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
};