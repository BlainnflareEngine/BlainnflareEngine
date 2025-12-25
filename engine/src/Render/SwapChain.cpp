#include "pch.h"

#include "Render/SwapChain.h"
#include "Render/Device.h"
#include "Render/CommandQueue.h"
#include "comdef.h"

namespace Blainn
{
    SwapChain::SwapChain(HWND hWnd, DXGI_FORMAT backBufferFormat)
        : m_hWnd(hWnd)
        , m_width(0u)
        , m_height(0u)
        , m_backBufferFormat(backBufferFormat)
        , m_vsync(true)
        , m_tearingSupported(false)
        , m_fullscreen(false)
    {
        assert(hWnd);
        // Check for tearing support.
        /*BOOL allowTearing = FALSE;
        if (SUCCEEDED(dxgiFactory5->CheckFeatureSupport(DXGI_FEATURE_PRESENT_ALLOW_TEARING, &allowTearing, sizeof(BOOL))))
        {
            m_tearingSupported = (allowTearing == TRUE);
        }*/

        RECT rect;
        GetClientRect(hWnd, &rect);

        m_width = rect.right - rect.left;
        m_height = rect.bottom - rect.top;

        m_dxgiSwapChain.Reset();

        // Describe and create the swap chain.
        // DXGI_SWAP_CHAIN_DESC sd;
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = m_width;
        swapChainDesc.Height = m_height;
        swapChainDesc.Format = backBufferFormat; // Back buffer format
        swapChainDesc.Stereo = FALSE;                                          
        swapChainDesc.SampleDesc = {1u, 0u}; // No MSAA
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = SwapChainFrameCount;
        //swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapChainDesc.Flags = m_tearingSupported ? DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING : 0u;
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullScreenDesc = {};
        swapChainFullScreenDesc.RefreshRate.Numerator = 60u;
        swapChainFullScreenDesc.RefreshRate.Denominator = 1u;
        swapChainFullScreenDesc.Windowed = TRUE;

        auto &device = Device::GetInstance();
        auto commandQueue = device.GetCommandQueue();
        
        auto factory = device.GetFactory();

        ComPtr<IDXGISwapChain1> swapChain;
        
        ThrowIfFailed(factory->CreateSwapChainForHwnd(
            commandQueue->GetCommandQueue().Get(), // Swap chain needs the queue so that it can force a flush on it.
            hWnd, &swapChainDesc, &swapChainFullScreenDesc, nullptr, &swapChain));

        HRESULT hr = device.GetDevice2()->GetDeviceRemovedReason();
        auto error = _com_error(hr);
        BF_ERROR("{}", error.ErrorMessage());

        // This sample does not support fullscreen transitions.
        ThrowIfFailed(factory->MakeWindowAssociation(hWnd, DXGI_MWA_NO_ALT_ENTER));

        ThrowIfFailed(swapChain.As(&m_dxgiSwapChain));

        //ResetRenderTargets();
    }

    SwapChain::~SwapChain()
    {
        
    }

    void SwapChain::SetFullscreen(bool fullscreen)
    {
        if (m_fullscreen != fullscreen)
        {
            m_fullscreen = fullscreen;
            // TODO:
            // m_dxgiSwapChain->SetFullscreenState()
        }
    }

    //void SwapChain::WaitForSwapChain()
    //{
    //    DWORD result = ::WaitForSingleObjectEx(m_hFrameLatencyWaitableObject, 1000,
    //                                           TRUE); // Wait for 1 second (should never have to wait that long...)
    //}

    void SwapChain::Reset(UINT width, UINT height)
    {
        if (m_width != width || m_height != height)
        {
            m_width = std::max(1u, width);
            m_height = std::max(1u, height);

            for (UINT i = 0; i < SwapChainFrameCount; ++i)
            {
                m_renderTargets[i].Reset();
            }

            DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
            ThrowIfFailed(m_dxgiSwapChain->GetDesc(&swapChainDesc));
            ThrowIfFailed(m_dxgiSwapChain->ResizeBuffers(SwapChainFrameCount, m_width, m_height,
                                                         swapChainDesc.BufferDesc.Format, swapChainDesc.Flags));
            
            m_currBackBuffer = m_dxgiSwapChain->GetCurrentBackBufferIndex();

            ResetRenderTargets();
        }
    }

    ID3D12Resource *SwapChain::GetBackBuffer() const
    {
        return m_renderTargets[m_currBackBuffer].Get();
    }

    VOID SwapChain::Present()
    {
        /*UINT syncInterval = m_VSync ? 1 : 0;
        UINT presentFlags = m_IsTearingSupported && !m_VSync ? DXGI_PRESENT_ALLOW_TEARING : 0;*/

        ThrowIfFailed(m_dxgiSwapChain->Present(1u, 0u));
        m_currBackBuffer = m_dxgiSwapChain->GetCurrentBackBufferIndex();
    }

    void SwapChain::ResetRenderTargets()
    {
        auto rtvHeap = Device::GetInstance().GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        auto rtvDescriptorSize = Device::GetInstance().GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());

        for (UINT i = 0; i < SwapChainFrameCount; ++i)
        {
            ThrowIfFailed(m_dxgiSwapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));
            Device::GetInstance().CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHeapHandle);
            rtvHeapHandle.Offset(1, rtvDescriptorSize);

            std::wstring name = L"Backbuffer[" + std::to_wstring(i) + L"]";
            m_renderTargets[i]->SetName(name.c_str());
        }
    }
} // namespace Blainn