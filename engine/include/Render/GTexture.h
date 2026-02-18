//
// Created by WhoLeb on 11-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once
#include <DirectXTK12/Src/d3dx12.h>
#include <EASTL/shared_ptr.h>
#include "Resource.h"

namespace Blainn
{

class Device;

class GTexture : public Resource
{
public:
    struct ResizeRequest
    {
        UINT width = 1u;
        UINT height = 1u;
        UINT depthOrArraySize = 1u;
    };

    GTexture(Device& device, const D3D12_RESOURCE_DESC& resourceDesc, const D3D12_CLEAR_VALUE* clearValue = nullptr);
    GTexture(Device& device, const ComPtr<ID3D12Resource>& resource, const D3D12_CLEAR_VALUE* clearValue = nullptr);

    virtual ~GTexture();

    void Resize(const ResizeRequest &request);

    D3D12_CPU_DESCRIPTOR_HANDLE GetRenderTargetView() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetDepthStencilVeiw() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetShaderResourceView() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetUnorderedAccessView(UINT mip) const;

    bool CheckSRVSupport() const
    {
        return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_SHADER_SAMPLE);
    }

    bool CheckRTVSupport() const
    {
        return CheckFormatSupport(D3D12_FORMAT_SUPPORT1_RENDER_TARGET);
    }

    bool CheckUAVSupport() const
    {
        return CheckFormatSupport( D3D12_FORMAT_SUPPORT1_TYPED_UNORDERED_ACCESS_VIEW ) &&
               CheckFormatSupport( D3D12_FORMAT_SUPPORT2_UAV_TYPED_LOAD ) &&
               CheckFormatSupport( D3D12_FORMAT_SUPPORT2_UAV_TYPED_STORE );
    }

    bool CheckDSVSupport() const
    {
        return CheckFormatSupport( D3D12_FORMAT_SUPPORT1_DEPTH_STENCIL );
    }

    void CreateRenderTargetView(const eastl::shared_ptr<D3D12_RENDER_TARGET_VIEW_DESC>& rtvDesc = nullptr);
    void CreateDepthStencilView(const eastl::shared_ptr<D3D12_DEPTH_STENCIL_VIEW_DESC>& dsvDesc = nullptr);
    void CreateShaderResourceView(const eastl::shared_ptr<D3D12_SHADER_RESOURCE_VIEW_DESC>& srvDesc = nullptr);
    void CreateUnorderedAccessView(const eastl::shared_ptr<D3D12_UNORDERED_ACCESS_VIEW_DESC>& uavDesc = nullptr);

private:
    void CreateViews();

private:
    CD3DX12_CPU_DESCRIPTOR_HANDLE m_RTVCPUHandle;
    CD3DX12_CPU_DESCRIPTOR_HANDLE m_DSVCPUHandle;
    CD3DX12_CPU_DESCRIPTOR_HANDLE m_SRVCPUHandle;
    CD3DX12_CPU_DESCRIPTOR_HANDLE m_UAVCPUHandle;

    eastl::shared_ptr<D3D12_RENDER_TARGET_VIEW_DESC> m_RTVDesc = nullptr;
    eastl::shared_ptr<D3D12_DEPTH_STENCIL_VIEW_DESC> m_DSVDesc = nullptr;
    eastl::shared_ptr<D3D12_SHADER_RESOURCE_VIEW_DESC> m_SRVDesc = nullptr;
    eastl::shared_ptr<D3D12_UNORDERED_ACCESS_VIEW_DESC> m_UAVDesc = nullptr;
};

}
