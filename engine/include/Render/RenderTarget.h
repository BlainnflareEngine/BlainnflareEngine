//
// Created by WhoLeb on 11-Jan-26.
// Copyright (c) 2026 Blainnflare. All rights reserved.
//

#pragma once
#include <cstdint>
#include <DirectXMath.h>
#include <intsafe.h>

#include "D3D12MemAlloc.h"
#include "EASTL/shared_ptr.h"
#include "EASTL/vector.h"

namespace Blainn
{

class Texture;

struct AttachmentPoint
{
    enum Enum : uint8_t
    {
        Color0,
        Color1,
        Color2,
        Color3,
        Color4,
        Color5,
        Color6,
        Color7,
        DepthStencil,
        NumAttachmentPoints
    };
};

class RenderTarget
{
public:
    RenderTarget();
    RenderTarget(const RenderTarget& copy) = default;
    RenderTarget(      RenderTarget&& copy) = default;

    RenderTarget& operator=(const RenderTarget& other) = default;
    RenderTarget& operator=(      RenderTarget&& other) = default;

    void AttachTexture(AttachmentPoint::Enum attachmentPoint, eastl::shared_ptr<Texture> texture);
    eastl::shared_ptr<Texture> GetTexture(AttachmentPoint::Enum attachmentPoint) const;

    void Resize(UINT width, UINT height);
    UINT GetWidth() const;
    UINT GetHeight() const;

    D3D12_VIEWPORT GetViewport(DirectX::XMFLOAT2 scale = {1.f, 1.f}, DirectX::XMFLOAT2 bias = {0.f, 0.f},
                                                            float minDepth = 0.f, float maxDepth = 0.f) const;

    const eastl::vector<eastl::shared_ptr<Texture>>& GetTextures() const;

    D3D12_RT_FORMAT_ARRAY GetRenderTargetFormats() const;

    DXGI_FORMAT GetDepthStencilFormat() const;

    DXGI_SAMPLE_DESC GetSampleDesc() const;

    void Reset()
    {
        m_Textures = RenderTargetList(AttachmentPoint::NumAttachmentPoints);
    }

private:
    using RenderTargetList = eastl::vector<eastl::shared_ptr<Texture>>;
    RenderTargetList m_Textures;

    UINT m_width, m_height;
};
}