
#include "Render/GBuffer.h"

GBuffer::GBuffer(ID3D12Device *device, UINT width, UINT height)
    : m_device(device)
    , m_width(width)
    , m_height(height)
{
    CreateResources();
}

GBuffer::~GBuffer() noexcept
{
}

void GBuffer::OnResize(UINT newWidth, UINT newHeight)
{
    if (m_width != newWidth || m_height != newHeight)
    {
        m_width = newWidth;
        m_height = newHeight;

        CreateResources();

        // New resource, so we need new descriptors to that resource.
        CreateDescriptors();
    }
}

ID3D12Resource *GBuffer::Get(unsigned layer)
{
    return m_buffer[layer].m_resource.Get();
}

FGBufferTexture *GBuffer::GetBufferTexture(unsigned layer)
{
    return &m_buffer[layer];
}

DXGI_FORMAT GBuffer::GetBufferTextureFormat(unsigned layer)
{
    return m_bufferFormats[layer];
}

CD3DX12_GPU_DESCRIPTOR_HANDLE GBuffer::GetSrv(unsigned layer) const
{
    return m_buffer[layer].m_hGpuSrv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE GBuffer::GetRtv(unsigned layer) const
{
    assert(layer != EGBufferLayer::DEPTH);
    return m_buffer[layer].m_hCpuRtvDsv;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE GBuffer::GetDsv(unsigned layer) const
{
    assert(layer == EGBufferLayer::DEPTH);
    return m_buffer[layer].m_hCpuRtvDsv;
}

void GBuffer::SetDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuSrv, CD3DX12_GPU_DESCRIPTOR_HANDLE hGpuSrv,
                             CD3DX12_CPU_DESCRIPTOR_HANDLE hCpuRtvDsv, unsigned layer)
{
    m_buffer[layer].m_hCpuSrv = hCpuSrv;
    m_buffer[layer].m_hGpuSrv = hGpuSrv;
    m_buffer[layer].m_hCpuRtvDsv = hCpuRtvDsv;
}

void GBuffer::CreateDescriptors()
{
    // Create SRV to resource so we can sample the GBuffer texture in a shader program.
    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    ZeroMemory(&srvDesc, sizeof(D3D12_SHADER_RESOURCE_VIEW_DESC));
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MipLevels = 1u;

    // Create SRVs and RTVs for texture of GBuffer that aren't depth
    for (UINT i = 0; i < static_cast<UINT>(EGBufferLayer::DEPTH); i++)
    {
        srvDesc.Format = m_bufferFormats[i];
        m_device->CreateShaderResourceView(m_buffer[i].m_resource.Get(), &srvDesc, m_buffer[i].m_hCpuSrv);

        D3D12_RENDER_TARGET_VIEW_DESC rtvDesc = {};
        ZeroMemory(&rtvDesc, sizeof(rtvDesc));
        rtvDesc.Format = m_bufferFormats[i];
        rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
        rtvDesc.Texture2D.MipSlice = 0u;
        rtvDesc.Texture2D.PlaneSlice = 0u;
        m_device->CreateRenderTargetView(m_buffer[i].m_resource.Get(), &rtvDesc, m_buffer[i].m_hCpuRtvDsv);
    }

    // Create SRV and DSV for depth texture
    auto depthIndex = static_cast<UINT>(EGBufferLayer::DEPTH);
    srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
    m_device->CreateShaderResourceView(m_buffer[depthIndex].m_resource.Get(), &srvDesc, m_buffer[depthIndex].m_hCpuSrv);

    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = m_bufferFormats[depthIndex];
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Texture2D.MipSlice = 0u;
    m_device->CreateDepthStencilView(m_buffer[depthIndex].m_resource.Get(), &dsvDesc,
                                     m_buffer[depthIndex].m_hCpuRtvDsv);
}

void GBuffer::CreateResources()
{
    CD3DX12_RESOURCE_DESC texDesc = {};
    ZeroMemory(&texDesc, sizeof(CD3DX12_RESOURCE_DESC));
    texDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    texDesc.Alignment = (UINT64)0;
    texDesc.Width = (UINT64)m_width;
    texDesc.Height = m_height;
    texDesc.DepthOrArraySize = (UINT16)1;
    texDesc.MipLevels = (UINT16)0;
    texDesc.SampleDesc.Count = 1u;
    texDesc.SampleDesc.Quality = 0u;
    texDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;

    D3D12_CLEAR_VALUE optClear = {};
    ZeroMemory(&optClear, sizeof(D3D12_CLEAR_VALUE));

    // Create resources for GBuffer that aren't depth
    for (UINT i = 0; i < static_cast<UINT>(EGBufferLayer::DEPTH); i++)
    {
        texDesc.Format = m_bufferFormats[i];
        texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

        optClear.Format = texDesc.Format;

        if (i == EGBufferLayer::DIFFUSE_ALBEDO)
            memcpy(&optClear.Color[0], &Colors::LightSteelBlue, sizeof(optClear.Color));
        else memcpy(&optClear.Color[0], &m_optimizedClearColor[0], sizeof(optClear.Color));

        auto uploadHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

        ThrowIfFailed(m_device->CreateCommittedResource(&uploadHeap, D3D12_HEAP_FLAG_NONE, &texDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ, &optClear, IID_PPV_ARGS(&m_buffer[i].m_resource)));
    }

    auto depthIndex = static_cast<UINT>(EGBufferLayer::DEPTH);
    // Create resource for depth
    texDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
    texDesc.Format = m_bufferFormats[depthIndex];

    optClear.Format = m_bufferFormats[depthIndex];
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = (UINT8)0;

    auto defaultHeap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);

    ThrowIfFailed(m_device->CreateCommittedResource(&defaultHeap,
                                                    D3D12_HEAP_FLAG_NONE, &texDesc, D3D12_RESOURCE_STATE_GENERIC_READ,
                                                    &optClear, IID_PPV_ARGS(&m_buffer[depthIndex].m_resource)));
}