//
// Created by WhoLeb on 11-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#include "Render/Texture.h"

#include "Render/Device.h"

using namespace Blainn;

Texture::Texture(Device &device, const D3D12_RESOURCE_DESC &resourceDesc, const D3D12_CLEAR_VALUE *clearValue)
    : Resource(device, resourceDesc, clearValue)
{
    CreateViews();
}

Texture::Texture(Device &device, const ComPtr<ID3D12Resource> &resource, const D3D12_CLEAR_VALUE *clearValue)
    : Resource(device, resource, clearValue)
{
    CreateViews();
}

void Texture::Resize(UINT width, UINT height, UINT depthOrArraySize)
{
    if (!m_d3d12Resource)
    {
        BF_INFO("Resizing empty texture");
        return;
    }

    CD3DX12_RESOURCE_DESC resDesc(m_d3d12Resource->GetDesc());

    resDesc.Width =  eastl::max(width, 1u);
    resDesc.Height = eastl::max(height, 1u);
    resDesc.DepthOrArraySize = depthOrArraySize;

    auto d3d12Device = m_Device.GetDevice2();

    auto heapDesc = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    ThrowIfFailed(d3d12Device->CreateCommittedResource(
        &heapDesc,
        D3D12_HEAP_FLAG_NONE,
        &resDesc,
        D3D12_RESOURCE_STATE_COMMON,
        m_ClearValue.get(),
        IID_PPV_ARGS(&m_d3d12Resource)
        ));

    m_d3d12Resource->SetName(m_Name.c_str());

    CreateViews();
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetRenderTargetView() const
{
    return m_RTVCPUHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetDepthStencilVeiw() const
{
    return m_DSVCPUHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetShaderResourceView() const
{
    return m_SRVCPUHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE Texture::GetUnorderedAccessView(UINT mip) const
{
    BF_ERROR("GetUnorderedAccessView unimplemented");
    return m_UAVCPUHandle;
}

void Texture::CreateViews()
{
    if (!m_d3d12Resource)
    {
        BF_WARN("Attempting to create views while there is no resource");
        return;
    }

    auto d3d12Device = m_Device.GetDevice2();

    CD3DX12_RESOURCE_DESC desc(m_d3d12Resource->GetDesc());

    if (m_RTVDesc || desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET && CheckSRVSupport())
        d3d12Device->CreateRenderTargetView(m_d3d12Resource.Get(), m_RTVDesc.get(), m_RTVCPUHandle);

    if (m_DSVDesc || desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL && CheckDSVSupport())
        d3d12Device->CreateDepthStencilView(m_d3d12Resource.Get(), m_DSVDesc.get(), m_DSVCPUHandle);

    if (m_SRVDesc || (desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) == 0 && CheckSRVSupport())
        d3d12Device->CreateShaderResourceView(m_d3d12Resource.Get(), m_SRVDesc.get(), m_SRVCPUHandle);

    // TODO create UAV as well as for mips
}
