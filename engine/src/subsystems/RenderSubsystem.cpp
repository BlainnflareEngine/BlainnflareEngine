#include "subsystems/RenderSubsystem.h"
#include "Render/FreyaMath.h"

#include "subsystems/Log.h"
#include "tools/Profiler.h"

#include <cassert>
#include <iostream>

using namespace Blainn;

RenderSubsystem &Blainn::RenderSubsystem::GetInstance()
{
    static RenderSubsystem render;
    return render;
}

void Blainn::RenderSubsystem::Init(HWND window)
{
    if (m_isInitialized) return;
    BF_INFO("RenderSubsystem::Init()");
    
    RECT rect;
    GetClientRect(window, &rect);

    m_width = rect.right - rect.left;
    m_height = rect.bottom - rect.top;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    // Blainn::Device::CreateDebugLayer();
#endif
    // m_renderer->Init();

    m_isInitialized = true;
}


void Blainn::RenderSubsystem::Destroy()
{
}

void Blainn::RenderSubsystem::Render(float deltaTime)
{
    //BLAINN_PROFILE_THREAD("Render thread");
    assert(m_isInitialized && "Freya subsystem not initialized");

    BLAINN_PROFILE_SCOPE_DYNAMIC("Render function");
    BF_INFO("RenderSubsystem::Render()");

    // Record all the commands we need to render the scene into the command list.
    //m_renderer->PopulateCommandList();
    
    //m_renderer->ExecuteCommandLists();
    
    // Present the frame.
    //ThrowIfFailed(m_swapChain->Present(1u, 0u));

    //MoveToNextFrame();
}

VOID Blainn::RenderSubsystem::MoveToNextFrame()
{
    // Schedule a Signal command in the queue.
    const UINT64 currentFenceValue = m_fenceValues[m_frameIndex];
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), currentFenceValue));

    // Update the frame index.
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();

    // If the next frame is not ready to be rendered yet, wait until it is ready.
    if (m_fence->GetCompletedValue() < m_fenceValues[m_frameIndex])
    {
        ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
        WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);
    }

    // Set the fence value for the next frame.
    m_fenceValues[m_frameIndex] = currentFenceValue + 1;
}

std::array<const CD3DX12_STATIC_SAMPLER_DESC, 5> Blainn::RenderSubsystem::GetStaticSamplers()
{
    const CD3DX12_STATIC_SAMPLER_DESC pointWrap(
        0u, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_POINT, // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

    const CD3DX12_STATIC_SAMPLER_DESC linearWrap(
        1u, // shaderRegister
        D3D12_FILTER_MIN_MAG_MIP_LINEAR, // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

    const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(
        2u, // shaderRegister
        D3D12_FILTER_ANISOTROPIC, // filter
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
        D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressW
        0.0f,                             // mipLODBias
        8u);                              // maxAnisotropy

    const CD3DX12_STATIC_SAMPLER_DESC shadowSampler(
        3u,
        D3D12_FILTER_MIN_MAG_MIP_LINEAR,
        D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        0.0f,
        16u
        );

    const CD3DX12_STATIC_SAMPLER_DESC shadowComparison(
        4u,
        D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR,
        D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        0.0f,
        16u,
        D3D12_COMPARISON_FUNC_LESS_EQUAL,
        D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK
    );

    return { pointWrap, linearWrap, anisotropicWrap, shadowSampler, shadowComparison };
}

#ifdef max
    #undef max
#endif
#ifdef min
    #undef min
#endif


std::pair<XMMATRIX, XMMATRIX> Blainn::RenderSubsystem::GetLightSpaceMatrix(const float nearZ, const float farZ)
{
    const auto directionalLight = m_mainPassCBData.DirLight;

    const XMFLOAT3 lightDir = directionalLight.Direction;

    const auto cameraProj = XMMatrixPerspectiveFovLH(m_camera->GetFovYRad(), m_aspectRatio, nearZ, farZ);
    const auto frustumCorners = GetFrustumCornersWorldSpace(m_camera->GetViewMatrix(), cameraProj);

    XMVECTOR center = XMVectorZero();
    for (const auto& v : frustumCorners)
    {
        center += v;
    }
    center /= (float)frustumCorners.size();

    const XMMATRIX lightView = XMMatrixLookAtLH(center, center + XMVectorSet(lightDir.x, lightDir.y, lightDir.z, 1.0f), FreyaMath::UpVector);

    // Measuring cascade
    float minX = eastl::numeric_limits<float>::max();
    float minY = eastl::numeric_limits<float>::max();
    float minZ = eastl::numeric_limits<float>::max();
    float maxX = eastl::numeric_limits<float>::lowest();
    float maxY = eastl::numeric_limits<float>::lowest();
    float maxZ = eastl::numeric_limits<float>::lowest();

    for (const auto& v : frustumCorners)
    {
        const auto trf = XMVector4Transform(v, lightView);

        minX = eastl::min(minX, XMVectorGetX(trf));
        maxX = eastl::max(maxX, XMVectorGetX(trf));
        minY = eastl::min(minY, XMVectorGetY(trf));
        maxY = eastl::max(maxY, XMVectorGetY(trf));
        minZ = eastl::min(minZ, XMVectorGetZ(trf));
        maxZ = eastl::max(maxZ, XMVectorGetZ(trf));
    }
    // Tune this parameter according to the scene
    constexpr float zMult = 10.0f;
    minZ = (minZ < 0) ? minZ * zMult : minZ / zMult;
    maxZ = (maxZ < 0) ? maxZ / zMult : maxZ * zMult;

    const XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(minX, maxX, minY, maxY, minZ, maxZ);

    return std::make_pair(lightView, lightProj);
}

void Blainn::RenderSubsystem::GetLightSpaceMatrices(std::vector<std::pair<XMMATRIX, XMMATRIX>>& outMatrices)
{
    for (UINT i = 0; i < MaxCascades; ++i)
    {
        if (i == 0)
        {
            outMatrices.push_back(GetLightSpaceMatrix(m_camera->GetNearZ(), m_shadowCascadeLevels[i]));
        }
        else if (i < MaxCascades - 1)
        {
            outMatrices.push_back(GetLightSpaceMatrix(m_shadowCascadeLevels[i - 1], m_shadowCascadeLevels[i]));
        }
        else
        {
            outMatrices.push_back(GetLightSpaceMatrix(m_shadowCascadeLevels[i - 1], m_shadowCascadeLevels[i]));
        }
    }
}

void Blainn::RenderSubsystem::CreateShadowCascadeSplits()
{
    const float minZ = m_camera->GetNearZ();
    const float maxZ = m_camera->GetFarZ();

    const float range = maxZ - minZ;
    const float ratio = maxZ / minZ;

    for (int i = 0; i < MaxCascades; i++)
    {
        float p = (i + 1) / (float)(MaxCascades);
        float log = (float)(minZ * pow(ratio, p));
        float uniform = minZ + range * p;
        float d = 0.95f * (log - uniform) + uniform; // 0.95f - idk, just magic value
        m_shadowCascadeLevels[i] = ((d - minZ) / range) * maxZ;
    }
}

std::vector<XMVECTOR> Blainn::RenderSubsystem::GetFrustumCornersWorldSpace(const XMMATRIX& view, const XMMATRIX& proj)
{
    const auto viewProj = view * proj;

    XMVECTOR det = XMMatrixDeterminant(viewProj);
    const auto invViewProj = XMMatrixInverse(&det, viewProj);

    std::vector<XMVECTOR> frustumCorners;
    frustumCorners.reserve(8);

    for (UINT x = 0; x < 2; ++x)
    {
        for (UINT y = 0; y < 2; ++y)
        {
            for (UINT z = 0; z < 2; ++z)
            {
                // translate NDC coords to world space
                const XMVECTOR pt = XMVector4Transform(
                    XMVectorSet(
                        2.0f * x - 1.0f,
                        2.0f * y - 1.0f,
                        (float)z,
                        1.0f), invViewProj);
                frustumCorners.push_back(pt / XMVectorGetW(pt));
            }
        }
    }
    return frustumCorners;
}