//
// Created by WhoLeb on 11-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#include "Render/Resource.h"

#include "Render/Device.h"

using namespace Blainn;

Resource::Resource(Device &device, const D3D12_RESOURCE_DESC &resourceDesc, const D3D12_CLEAR_VALUE *clearValue)
    : m_Device(device)
{
    auto d3d12Device = m_Device.GetDevice2();
    if (clearValue)
        m_ClearValue = eastl::make_unique<D3D12_CLEAR_VALUE>(*clearValue);

    auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
    if (FAILED(d3d12Device->CreateCommittedResource(
        &heapProps,
        D3D12_HEAP_FLAG_NONE,
        &resourceDesc,
        D3D12_RESOURCE_STATE_COMMON,
        m_ClearValue.get(),
        IID_PPV_ARGS(&m_d3d12Resource)
        )))
    {
        BF_ERROR("Failed to create a committed resource");
        return;
    }

    CheckFeatureSupport();
}

Resource::Resource(Device &device, Microsoft::WRL::ComPtr<ID3D12Resource> resource, const D3D12_CLEAR_VALUE *clearValue)
    : m_Device(device)
    , m_d3d12Resource(resource)
{
    if (clearValue)
        m_ClearValue = eastl::make_unique<D3D12_CLEAR_VALUE>(*clearValue);

    CheckFeatureSupport();
}

D3D12_RESOURCE_DESC Resource::GetD3D12ResourceDesc() const
{
    D3D12_RESOURCE_DESC resDesc = {};
    if ( m_d3d12Resource )
    {
        resDesc = m_d3d12Resource->GetDesc();
    }

    return resDesc;
}

void Resource::SetName(const eastl::wstring &name)
{
    m_Name = name;
    if (m_d3d12Resource && !m_Name.empty())
        m_d3d12Resource->SetName(m_Name.c_str());
}

bool Resource::CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const
{
    return ( m_FormatSupport.Support1 & formatSupport ) != 0;
}

bool Resource::CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const
{
    return ( m_FormatSupport.Support2 & formatSupport ) != 0;
}

void Resource::CheckFeatureSupport()
{
    auto d3d12Device = m_Device.GetDevice2();

    auto desc              = m_d3d12Resource->GetDesc();
    m_FormatSupport.Format = desc.Format;
    ThrowIfFailed( d3d12Device->CheckFeatureSupport( D3D12_FEATURE_FORMAT_SUPPORT, &m_FormatSupport,
                                                     sizeof( D3D12_FEATURE_DATA_FORMAT_SUPPORT ) ) );
}