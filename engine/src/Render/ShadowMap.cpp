#include "Render/ShadowMap.h"

ShadowMap::ShadowMap(ID3D12Device *device, UINT width, UINT height, UINT cascadesCount)
    : m_device(device)
    , m_mapWidth(width)
    , m_mapHeight(height)
    , m_cascadesCount(cascadesCount)
{
    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
    m_viewport.Width = static_cast<FLOAT>(width);
    m_viewport.Height = static_cast<FLOAT>(height);
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

    m_scissorRect.left = 0L;
    m_scissorRect.top = 0L;
    m_scissorRect.right = static_cast<LONG>(width);
    m_scissorRect.bottom = static_cast<LONG>(height);

    if (m_cascadesCount == 0)
    {
        CreateResource();
    }
}

ShadowMap::~ShadowMap() noexcept
{
}

ID3D12Resource *ShadowMap::Get()
{
    return m_shadowMap.Get();
}

void ShadowMap::CreateDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv, CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuDsv)
{
    m_hCpuSrv = hCpuSrv;
    m_hGpuSrv = hGpuSrv;
    m_hCpuDsv = hCpuDsv;

    CreateDescriptors();
}

void ShadowMap::OnResize(UINT newWidth, UINT newHeight)
{
    if ((m_mapWidth != newWidth) || (m_mapHeight != newHeight))
    {
        m_mapWidth = newWidth;
        m_mapHeight = newHeight;

        CreateResource();

        // New resource, so we need new descriptors to that resource.
        CreateDescriptors();
    }
}

void ShadowMap::CreateDescriptors()
{
    // Create SRV to resource so we can sample the shadow map in a shader program.
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0u;
    srvDesc.Texture2D.MipLevels = 1u;
    srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;
    srvDesc.Texture2D.PlaneSlice = 0u;
    m_device->CreateShaderResourceView(m_shadowMap.Get(), &srvDesc, m_hCpuSrv);

    // Create DSV to resource so we can render to the shadow map.
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
    dsvDesc.Texture2D.MipSlice = 0u;
    m_device->CreateDepthStencilView(m_shadowMap.Get(), &dsvDesc, m_hCpuDsv);
}

void ShadowMap::CreateResource()
{
    D3D12_RESOURCE_DESC textureDesc;
    ZeroMemory(&textureDesc, sizeof(D3D12_RESOURCE_DESC));

    textureDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    textureDesc.Alignment = (UINT64)0;
    textureDesc.Width = (UINT64)m_mapWidth;
    textureDesc.Height = m_mapHeight;
    textureDesc.DepthOrArraySize = (UINT16)1;
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

    m_shadowMap->SetName(L"Shadow Map");
}