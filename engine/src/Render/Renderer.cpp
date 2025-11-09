#include "Render/Renderer.h"

Blainn::Renderer::Renderer(Device* device, uint32_t width, uint32_t height)
    : m_device(device)
    , m_width(width)
    , m_height(height)
{
}

Blainn::Renderer::~Renderer()
{
    m_device = nullptr;
}

void Blainn::Renderer::Init()
{    
    CreateCommandQueues();
    CreateCommandAllocators();
    CreateCommandLists();
}

void Blainn::Renderer::CreateCommandQueues()
{

}

void Blainn::Renderer::CreateCommandAllocators()
{

}

void Blainn::Renderer::CreateCommandLists()
{

}

void Blainn::Renderer::CreateSwapChain()
{
    // // Describe and create the swap chain.
    // //DXGI_SWAP_CHAIN_DESC sd;
    // DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    // swapChainDesc.Width = m_width;
    // swapChainDesc.Height = m_height;
    // swapChainDesc.Format = BackBufferFormat; // Back buffer format
    // swapChainDesc.SampleDesc.Count = m_is4xMsaaState ? 4u : 1u; // MSAA
    // swapChainDesc.SampleDesc.Quality = m_is4xMsaaState ? (m_4xMsaaQuality - 1u) : 0u; // MSAA
    // swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    // swapChainDesc.BufferCount = SwapChainFrameCount;
    // swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    // swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    // DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullScreenDesc = {};
    // swapChainFullScreenDesc.RefreshRate.Numerator = 60u;
    // swapChainFullScreenDesc.RefreshRate.Denominator = 1u;
    // swapChainFullScreenDesc.Windowed = TRUE;

    // ComPtr<IDXGISwapChain1> swapChain;
    // ThrowIfFailed(m_factory->CreateSwapChainForHwnd(
    //     m_commandQueue.Get(),        // Swap chain needs the queue so that it can force a flush on it.
    //     Win32App::GetHwnd(),
    //     &swapChainDesc,
    //     &swapChainFullScreenDesc,
    //     nullptr,
    //     &swapChain
    // ));

    // This sample does not support fullscreen transitions.
    // ThrowIfFailed(m_factory->MakeWindowAssociation(Win32App::GetHwnd(), DXGI_MWA_NO_ALT_ENTER));

    // ThrowIfFailed(swapChain.As(&m_swapChain));
    // m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void Blainn::Renderer::CreateRtvAndDsvDescriptorHeaps()
{

}