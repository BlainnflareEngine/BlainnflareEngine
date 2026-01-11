//
// Created by WhoLeb on 11-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#include "Render/RenderTarget.h"

#include "Render/Texture.h"

using namespace Blainn;

RenderTarget::RenderTarget()
    : m_Textures(AttachmentPoint::NumAttachmentPoints)
    , m_width(0)
    , m_height(0)
{
}

void RenderTarget::AttachTexture(AttachmentPoint::Enum attachmentPoint, eastl::shared_ptr<Texture> texture)
{
    m_Textures[attachmentPoint] = texture;

    if (texture && texture->GetD3D12Resource())
    {
        auto desc = texture->GetD3D12ResourceDesc();
        m_width = static_cast<UINT>(desc.Width);
        m_height = static_cast<UINT>(desc.Height);
    }
}

eastl::shared_ptr<Texture> RenderTarget::GetTexture(AttachmentPoint::Enum attachmentPoint) const
{
    return m_Textures[attachmentPoint];
}

void RenderTarget::Resize(UINT width, UINT height)
{
    m_width = width;
    m_height = height;
    for (auto& texture : m_Textures)
        if (texture)
            texture->Resize(m_width, m_height);
}

UINT RenderTarget::GetWidth() const
{
    return m_width;
}

UINT RenderTarget::GetHeight() const
{
    return m_height;
}

D3D12_VIEWPORT RenderTarget::GetViewport(DirectX::XMFLOAT2 scale, DirectX::XMFLOAT2 bias, float minDepth,
    float maxDepth) const
{
    UINT64 width  = 0;
    UINT   height = 0;

    for ( int i = AttachmentPoint::Color0; i <= AttachmentPoint::Color7; ++i )
    {
        auto texture = m_Textures[i];
        if ( texture )
        {
            auto desc = texture->GetD3D12ResourceDesc();
            width     = std::max( width, desc.Width );
            height    = std::max( height, desc.Height );
        }
    }

    D3D12_VIEWPORT viewport = {
        ( width * bias.x ),    // TopLeftX
        ( height * bias.y ),   // TopLeftY
        ( width * scale.x ),   // Width
        ( height * scale.y ),  // Height
        minDepth,              // MinDepth
        maxDepth               // MaxDepth
    };

    return viewport;
}

const eastl::vector<eastl::shared_ptr<Texture>> & RenderTarget::GetTextures() const
{
    return m_Textures;
}

D3D12_RT_FORMAT_ARRAY RenderTarget::GetRenderTargetFormats() const
{
    D3D12_RT_FORMAT_ARRAY rtvFormats = {};

    for ( int i = AttachmentPoint::Color0; i <= AttachmentPoint::Color7; ++i )
    {
        auto texture = m_Textures[i];
        if ( texture )
        {
            rtvFormats.RTFormats[rtvFormats.NumRenderTargets++] = texture->GetD3D12ResourceDesc().Format;
        }
    }

    return rtvFormats;
}

DXGI_FORMAT RenderTarget::GetDepthStencilFormat() const
{
    DXGI_FORMAT    dsvFormat           = DXGI_FORMAT_UNKNOWN;
    auto depthStencilTexture = m_Textures[AttachmentPoint::DepthStencil];
    if ( depthStencilTexture )
    {
        dsvFormat = depthStencilTexture->GetD3D12ResourceDesc().Format;
    }

    return dsvFormat;
}

DXGI_SAMPLE_DESC RenderTarget::GetSampleDesc() const
{
    DXGI_SAMPLE_DESC sampleDesc = { 1, 0 };
    for ( int i = AttachmentPoint::Color0; i <= AttachmentPoint::Color7; ++i )
    {
        auto texture = m_Textures[i];
        if ( texture )
        {
            sampleDesc = texture->GetD3D12ResourceDesc().SampleDesc;
            break;
        }
    }

    return sampleDesc;
}
