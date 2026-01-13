//
// Created by WhoLeb on 11-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#include "Render/GTexture.h"

#include "Render/Device.h"

using namespace Blainn;

GTexture::GTexture(Device &device, const D3D12_RESOURCE_DESC &resourceDesc, const D3D12_CLEAR_VALUE *clearValue)
    : Resource(device, resourceDesc, clearValue)
{
    CreateViews();
}

GTexture::GTexture(Device &device, const ComPtr<ID3D12Resource> &resource, const D3D12_CLEAR_VALUE *clearValue)
    : Resource(device, resource, clearValue)
{
    CreateViews();
}

GTexture::~GTexture()
{
}

void GTexture::Resize(UINT width, UINT height, UINT depthOrArraySize)
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

D3D12_CPU_DESCRIPTOR_HANDLE GTexture::GetRenderTargetView() const
{
    return m_RTVCPUHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE GTexture::GetDepthStencilVeiw() const
{
    return m_DSVCPUHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE GTexture::GetShaderResourceView() const
{
    return m_SRVCPUHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE GTexture::GetUnorderedAccessView(UINT mip) const
{
    BF_ERROR("GetUnorderedAccessView unimplemented");
    return m_UAVCPUHandle;
}

void GTexture::CreateRenderTargetView(const eastl::shared_ptr<D3D12_RENDER_TARGET_VIEW_DESC> &rtvDesc)
{
    // TODO do this properly when the descriptor allocator is created
}

void GTexture::CreateDepthStencilView(const eastl::shared_ptr<D3D12_DEPTH_STENCIL_VIEW_DESC> &dsvDesc)
{
    // TODO do this properly when the descriptor allocator is created
}

void GTexture::CreateShaderResourceView(const eastl::shared_ptr<D3D12_SHADER_RESOURCE_VIEW_DESC> &srvDesc)
{
    // TODO do this properly when the descriptor allocator is created
}

void GTexture::CreateUnorderedAccessView(const eastl::shared_ptr<D3D12_UNORDERED_ACCESS_VIEW_DESC> &uavDesc)
{
    // TODO do this properly when the descriptor allocator is created
}

void GTexture::CreateViews()
{
    if (!m_d3d12Resource)
    {
        BF_WARN("Attempting to create views while there is no resource");
        return;
    }

    auto d3d12Device = m_Device.GetDevice2();

    CD3DX12_RESOURCE_DESC desc(m_d3d12Resource->GetDesc());

    if (m_RTVDesc || desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET && CheckRTVSupport())
    {
        auto rtvHeap = Device::GetInstance().GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        auto rtvDescriptorSize = Device::GetInstance().GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
        rtvHeapHandle.Offset(9, rtvDescriptorSize);

        m_Device.CreateRenderTargetView(m_d3d12Resource.Get(), m_RTVDesc.get(), rtvHeapHandle);

        m_RTVCPUHandle = rtvHeapHandle;
    }

    if (m_DSVDesc || desc.Flags & D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL && CheckDSVSupport())
    {
        auto dsvHeap = Device::GetInstance().GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        auto dsvDescriptorSize = Device::GetInstance().GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHeapHandle(dsvHeap->GetCPUDescriptorHandleForHeapStart());
        dsvHeapHandle.Offset(9, dsvDescriptorSize);

        //m_Device.CreateDepthStencilView(m_d3d12Resource.Get(), m_DSVDesc.get(), m_DSVCPUHandle);
        d3d12Device->CreateDepthStencilView(m_d3d12Resource.Get(), m_DSVDesc.get(), dsvHeapHandle);

        m_DSVCPUHandle = dsvHeapHandle;
    }

    if (m_SRVDesc || (desc.Flags & D3D12_RESOURCE_FLAG_DENY_SHADER_RESOURCE) == 0 && CheckSRVSupport())
    {
        auto srvHeap = Device::GetInstance().GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        auto srvDescriptorSize = Device::GetInstance().GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        CD3DX12_CPU_DESCRIPTOR_HANDLE srvHeapHandle(srvHeap->GetCPUDescriptorHandleForHeapStart());
        srvHeapHandle.Offset(2100, srvDescriptorSize);

        d3d12Device->CreateShaderResourceView(m_d3d12Resource.Get(), m_SRVDesc.get(), srvHeapHandle);

        m_SRVCPUHandle = srvHeapHandle;
    }

    // TODO create UAV as well as for mips
}
