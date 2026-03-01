#pragma once

#include <d3d12.h>
#include <cstdint>
#include "DirectXTK12/Src/d3dx12.h"
#include "Render/DXHelpers.h"

class ShadowMap
{
public:
    struct ShadowMapDesc
    {
        UINT width = 0u;
        UINT height = 0u;
        UINT cascadesCount = 0u;
    };

    struct DescriptorHandles
    {
        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuSrv;
        CD3DX12_GPU_DESCRIPTOR_HANDLE gpuSrv;
        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuDsv;
    };

    ShadowMap(ID3D12Device *device, const ShadowMapDesc &desc);
    ShadowMap(const ShadowMap &lhs) = delete;
    ShadowMap &operator=(const ShadowMap &lhs) = delete;

    virtual ~ShadowMap() noexcept;

public:
    FORCEINLINE UINT GetWidth() const
    {
        return m_mapWidth;
    }
    FORCEINLINE UINT GetHeight() const
    {
        return m_mapHeight;
    }

    ID3D12Resource *Get();

    FORCEINLINE CD3DX12_GPU_DESCRIPTOR_HANDLE GetSrv() const
    {
        return m_hGpuSrv;
    }
    FORCEINLINE CD3DX12_CPU_DESCRIPTOR_HANDLE GetDsv() const
    {
        return m_hCpuDsv;
    }

    FORCEINLINE D3D12_VIEWPORT GetViewport() const
    {
        return m_viewport;
    }
    FORCEINLINE D3D12_RECT GetScissorRect() const
    {
        return m_scissorRect;
    }

    void CreateDescriptors(const DescriptorHandles &descriptorHandles);

    void OnResize(UINT newWidth, UINT newHeight);

protected:
    virtual void CreateDescriptors();

private:
    void CreateResource();

protected:
    ID3D12Device *m_device = nullptr;

    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;

    CD3DX12_CPU_DESCRIPTOR_HANDLE m_hCpuSrv;
    CD3DX12_GPU_DESCRIPTOR_HANDLE m_hGpuSrv;
    CD3DX12_CPU_DESCRIPTOR_HANDLE m_hCpuDsv;

    DXGI_FORMAT m_format = DXGI_FORMAT_R24G8_TYPELESS;

    UINT m_mapWidth = 0u;
    UINT m_mapHeight = 0u;

    UINT m_cascadesCount = 0u;

    // actual gpu resource
    ComPtr<ID3D12Resource> m_shadowMap = nullptr;
};
