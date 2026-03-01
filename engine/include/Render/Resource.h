//
// Created by WhoLeb on 11-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once
#include "DXHelpers.h"

namespace Blainn
{

class Device;

class Resource
{
public:
    Resource(Device& device, const D3D12_RESOURCE_DESC& resourceDesc,
        const D3D12_CLEAR_VALUE* clearValue = nullptr);

    Resource(Device& device, Microsoft::WRL::ComPtr<ID3D12Resource> resource,
        const D3D12_CLEAR_VALUE* clearValue = nullptr);

    virtual ~Resource() = default;

    Device& GetDevice() const { return m_Device; }

    Microsoft::WRL::ComPtr<ID3D12Resource> GetD3D12Resource() const { return m_d3d12Resource; }

    D3D12_RESOURCE_DESC GetD3D12ResourceDesc() const;

    void SetName(const eastl::wstring& name);
    const eastl::wstring& GetName() const { return m_Name; }

    bool CheckFormatSupport(D3D12_FORMAT_SUPPORT1 formatSupport) const;
    bool CheckFormatSupport(D3D12_FORMAT_SUPPORT2 formatSupport) const;

private:
    void CheckFeatureSupport();

protected:
    Device& m_Device;

    Microsoft::WRL::ComPtr<ID3D12Resource>  m_d3d12Resource;
    D3D12_FEATURE_DATA_FORMAT_SUPPORT       m_FormatSupport;
    eastl::unique_ptr<D3D12_CLEAR_VALUE>    m_ClearValue;
    eastl::wstring                          m_Name;
};
}