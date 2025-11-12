#include "subsystems/RenderSubsystem.h"

#include "Render/Device.h"
#include "Render/Renderer.h"

#include "subsystems/Log.h"
#include "tools/Profiler.h"

#include <cassert>
#include <iostream>

using namespace Blainn;

void Blainn::RenderSubsystem::Init(HWND window)
{
    if (m_isInitialized) return;
    BF_INFO("RenderSubsystem::Init()");
    
    m_hWND = window;

    RECT rect;
    GetClientRect(window, &rect);

    m_width = rect.right - rect.left;
    m_height = rect.bottom - rect.top;

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    Blainn::Device::CreateDebugLayer();
#endif
    CreateDeviceResources();

    m_renderer = eastl::make_unique<Renderer>(m_device, m_width, m_height);
    m_renderer->Init();

    m_isInitialized = true;
}


void Blainn::RenderSubsystem::Destroy()
{
    WaitForGPU();
    CloseHandle(m_fenceEvent);
}

RenderSubsystem &Blainn::RenderSubsystem::GetInstance()
{
    static RenderSubsystem render;
    return render;
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

void Blainn::RenderSubsystem::CreateDeviceResources()
{
    m_device = eastl::shared_ptr<Device>();
    m_device->CreateCommandQueues();

    CreateSwapChain();
}

void Blainn::RenderSubsystem::CreateSwapChain()
{
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = m_width;
    swapChainDesc.Height = m_height;
    swapChainDesc.Format = BackBufferFormat; // Back buffer format
    swapChainDesc.SampleDesc = {1u, 0u};     // Disable MSAA
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = SwapChainFrameCount;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullScreenDesc = {};
    swapChainFullScreenDesc.RefreshRate.Numerator = 60u;
    swapChainFullScreenDesc.RefreshRate.Denominator = 1u;
    swapChainFullScreenDesc.Windowed = TRUE;

    m_swapChain = m_device->CreateSwapChain(swapChainDesc, swapChainFullScreenDesc, m_hWND);
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

void Blainn::RenderSubsystem::CreateFence()
{
    // Create synchronization objects and wait until assets have been uploaded to the GPU.
    {
        ThrowIfFailed(m_device->GetDevicePtr()->CreateFence(m_fenceValues[m_frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

        m_fenceValues[m_frameIndex]++;
        // Create an event handle to use for frame synchronization.
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvent == nullptr)
        {
            ThrowIfFailed(HRESULT_FROM_WIN32(GetLastError()));
        }
        // Wait for the command list to execute; we are reusing the same command
        // list in our main loop but for now, we just want to wait for setup to
        // complete before continuing.
        WaitForGPU();
    }
}

void Blainn::RenderSubsystem::WaitForGPU()
{
    // Schedule a Signal command in the queue.
    ThrowIfFailed(m_device->GetCommandQueue(ECommandQueueType::GFX)->Signal(m_fence.Get(), m_fenceValues[m_frameIndex]));

    // Wait until the fence has been processed.
    ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
    WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

    m_fenceValues[m_frameIndex]++;
}

void Blainn::RenderSubsystem::MoveToNextFrame()
{
    // Schedule a Signal command in the queue.
    const UINT64 currentFenceValue = m_fenceValues[m_frameIndex];
    ThrowIfFailed(m_device->GetCommandQueue(ECommandQueueType::GFX)->Signal(m_fence.Get(), currentFenceValue));

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
