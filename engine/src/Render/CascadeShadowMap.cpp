#include "Render/CascadeShadowMap.h"

CascadeShadowMap::CascadeShadowMap(ID3D12Device *device, const ShadowMapDesc &desc)
    : ShadowMap(device, desc)
{
    CreateResource();
}

CascadeShadowMap::~CascadeShadowMap() noexcept
{
}

void CascadeShadowMap::CreateDescriptors()
{
    // Create SRV to resource so we can sample the shadow map in a shader program.
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Texture2DArray.MostDetailedMip = 0u;
    srvDesc.Texture2DArray.MipLevels = UINT32_MAX;
    srvDesc.Texture2DArray.FirstArraySlice = 0u;
    srvDesc.Texture2DArray.ArraySize = m_shadowMap->GetDesc().DepthOrArraySize;
    srvDesc.Texture2DArray.PlaneSlice = 0u;
    srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
    m_device->CreateShaderResourceView(m_shadowMap.Get(), &srvDesc, m_hCpuSrv);

    // Create DSV to resource so we can render to the shadow map.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DARRAY;
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.Texture2DArray.MipSlice = 0u;
    dsvDesc.Texture2DArray.FirstArraySlice = 0u;
    dsvDesc.Texture2DArray.ArraySize = m_shadowMap->GetDesc().DepthOrArraySize;
    m_device->CreateDepthStencilView(m_shadowMap.Get(), &dsvDesc, m_hCpuDsv);
}

void CascadeShadowMap::CreateResource()
{
    D3D12_RESOURCE_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(D3D12_RESOURCE_DESC));

    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureDesc.Alignment = (UINT64)0;
    textureDesc.Width = (UINT64)m_mapWidth;
    textureDesc.Height = m_mapHeight;
    textureDesc.DepthOrArraySize = (UINT16)m_cascadesCount;
    textureDesc.MipLevels = (UINT16)1;
    textureDesc.Format = m_format;
    textureDesc.SampleDesc.Count = 1u;
    textureDesc.SampleDesc.Quality = 0u;

    textureDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    textureDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear;
    optClear.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = (UINT8)0;

    auto defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    ThrowIfFailed(m_device->CreateCommittedResource(&defaultHeap, D3D12_HEAP_FLAG_NONE, &textureDesc,
        D3D12_RESOURCE_STATE_GENERIC_READ, &optClear, IID_PPV_ARGS(&m_shadowMap)));
}
