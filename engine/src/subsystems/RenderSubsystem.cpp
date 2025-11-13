#include "pch.h"

#include "subsystems/RenderSubsystem.h"

#include <cassert>
#include <iostream>

#include "Render/FreyaMath.h"
#include "Render/Renderer.h"

#include "Engine.h"
#include "components/MeshComponent.h"
#include "components/RenderComponent.h"
#include "file-system/Model.h"
#include "handles/Handle.h"
#include "scene/Scene.h"
#include "subsystems/Log.h"
#include "tools/Profiler.h"


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
    CreateDebugLayer();
#endif
    InitializeD3D();

    // m_renderer = eastl::make_unique<Renderer>(m_device, m_width, m_height);
    // m_renderer->Init();

    m_isInitialized = true;
}

void Blainn::RenderSubsystem::Destroy()
{
    WaitForGPU();
    CloseHandle(m_fenceEvent);
    m_isInitialized = false;
    BF_INFO("RenderSubsystem::Destroy()");
}

Blainn::RenderSubsystem &Blainn::RenderSubsystem::GetInstance()
{
    static RenderSubsystem render;
    return render;
}

void Blainn::RenderSubsystem::Render(float deltaTime)
{
    // BLAINN_PROFILE_THREAD("Render thread");
    assert(m_isInitialized && "Freya subsystem not initialized");

    BLAINN_PROFILE_SCOPE_DYNAMIC("Render function");
    BF_INFO("RenderSubsystem::Render()");

    // Record all the commands we need to render the scene into the command list.
    // m_renderer->PopulateCommandList();

    Scene &scene = Engine::GetActiveScene();
    auto renderedEntities = scene.GetAllEntitiesWith<IDComponent, RenderComponent>();
    for (auto entityComponents : renderedEntities.each())
    {
        IDComponent &idComponent = std::get<1>(entityComponents);
        RenderComponent &renderComponent = std::get<2>(entityComponents);

        if (!renderComponent.m_visible || !renderComponent.m_meshCanBeRendered) continue;

        // TODO: render component
    }

    // m_renderer->ExecuteCommandLists();

    // Present the frame.
    // ThrowIfFailed(m_swapChain->Present(1u, 0u));

    MoveToNextFrame();
}

VOID Blainn::RenderSubsystem::InitializeD3D()
{
    CreateDevice();
    CreateCommandObjects();
    CreateFence();
    CreateRtvAndDsvDescriptorHeaps();
    CreateSwapChain();

    Reset();
}

VOID Blainn::RenderSubsystem::CreateDebugLayer()
{
    ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();

        // Enable additional debug layers.
        m_dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
}

// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
_Use_decl_annotations_ void Blainn::RenderSubsystem::GetHardwareAdapter(IDXGIFactory1 *pFactory,
                                                                        IDXGIAdapter1 **ppAdapter,
                                                                        bool requestHighPerformanceAdapter)
{
    *ppAdapter = nullptr;

    ComPtr<IDXGIAdapter1> adapter;

    ComPtr<IDXGIFactory6> factory6;
    if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
    {
        for (UINT adapterIndex = 0; SUCCEEDED(factory6->EnumAdapterByGpuPreference(
                 adapterIndex,
                 requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE
                                                       : DXGI_GPU_PREFERENCE_UNSPECIFIED,
                 IID_PPV_ARGS(&adapter)));
             ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    if (adapter.Get() == nullptr)
    {
        for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
        {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
            {
                // Don't select the Basic Render Driver adapter.
                // If you want a software adapter, pass in "/warp" on the command line.
                continue;
            }

            // Check to see whether the adapter supports Direct3D 12, but don't create the
            // actual device yet.
            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
            {
                break;
            }
        }
    }

    *ppAdapter = adapter.Detach();
}

// Helper function for setting the window's title text.
void Blainn::RenderSubsystem::SetCustomWindowText(LPCWSTR text) const
{
    // std::wstring windowText = m_title + L": " + text;
    // SetWindowText(m_hWND_, windowText.c_str());
}

VOID Blainn::RenderSubsystem::CreateDevice()
{
    ThrowIfFailed(CreateDXGIFactory2(m_dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)));

    if (m_useWarpDevice)
    {
        ComPtr<IDXGIAdapter> warpAdapter;
        ThrowIfFailed(m_factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
        ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
    }
    else
    {
        GetHardwareAdapter(m_factory.Get(), &m_hardwareAdapter);

        ThrowIfFailed(D3D12CreateDevice(m_hardwareAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device)));
    }
}

VOID Blainn::RenderSubsystem::CreateCommandObjects()
{
    // Describe and create the command queue.
    D3D12_COMMAND_QUEUE_DESC queueDesc = {};
    // the most often used are direct, compute and copy
    queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
    queueDesc.Priority;
    queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
    queueDesc.NodeMask;
    // If we have multiple command queues, we can write a resource only from one queue at the same time.
    // Before it can be accessed by another queue, it must transition to read or common state.
    // In a read state resource can be read from multiple command queues simultaneously, including across processes,
    // based on its read state.
    ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue)));

    ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                   IID_PPV_ARGS(&m_commandAllocators[m_frameIndex])));

    // Create the command list.
    ThrowIfFailed(
        m_device->CreateCommandList(0u /*Single GPU*/, D3D12_COMMAND_LIST_TYPE_DIRECT,
                                    m_commandAllocators[m_frameIndex].Get() /*Must match the command list type*/,
                                    nullptr, IID_PPV_ARGS(&m_commandList)));

    // Command lists are created in the recording state, but there is nothing
    // to record yet. The main loop expects it to be closed, so close it now.
    ThrowIfFailed(m_commandList->Close());
}

VOID Blainn::RenderSubsystem::CreateFence()
{
    // Create synchronization objects and wait until assets have been uploaded to the GPU.
    {
        ThrowIfFailed(
            m_device->CreateFence(m_fenceValues[m_frameIndex], D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)));

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

VOID Blainn::RenderSubsystem::CreateRtvAndDsvDescriptorHeaps()
{
    // Create descriptor heaps.
    // Descriptor heap has to be created for every GPU resource

    // Describe and create a render target view (RTV) descriptor heap.
    D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
    rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvHeapDesc.NumDescriptors = SwapChainFrameCount;
    rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvHeapDesc.NodeMask = 0u;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap)));

    m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {};
    dsvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
    dsvHeapDesc.NumDescriptors = 1u;
    dsvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    dsvHeapDesc.NodeMask = 0u;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvHeap)));

    m_dsvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

VOID Blainn::RenderSubsystem::CreateSwapChain()
{
    // Describe and create the swap chain.
    // DXGI_SWAP_CHAIN_DESC sd;
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.Width = m_width;
    swapChainDesc.Height = m_height;
    swapChainDesc.Format = BackBufferFormat;                                          // Back buffer format
    swapChainDesc.SampleDesc.Count = m_is4xMsaaState ? 4u : 1u;                       // MSAA
    swapChainDesc.SampleDesc.Quality = m_is4xMsaaState ? (m_4xMsaaQuality - 1u) : 0u; // MSAA
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.BufferCount = SwapChainFrameCount;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullScreenDesc = {};
    swapChainFullScreenDesc.RefreshRate.Numerator = 60u;
    swapChainFullScreenDesc.RefreshRate.Denominator = 1u;
    swapChainFullScreenDesc.Windowed = TRUE;

    ComPtr<IDXGISwapChain1> swapChain;
    ThrowIfFailed(m_factory->CreateSwapChainForHwnd(
        m_commandQueue.Get(), // Swap chain needs the queue so that it can force a flush on it.
        m_hWND, &swapChainDesc, &swapChainFullScreenDesc, nullptr, &swapChain));

    // This sample does not support fullscreen transitions.
    ThrowIfFailed(m_factory->MakeWindowAssociation(m_hWND, DXGI_MWA_NO_ALT_ENTER));

    ThrowIfFailed(swapChain.As(&m_swapChain));
    m_frameIndex = m_swapChain->GetCurrentBackBufferIndex();
}

VOID Blainn::RenderSubsystem::Reset()
{
    assert(m_device);
    assert(m_swapChain);

    // Before making any changes
    // FlushCommandQueue();

    ThrowIfFailed(m_commandList->Reset(m_commandAllocators[m_frameIndex].Get(), nullptr));

    for (UINT i = 0; i < SwapChainFrameCount; i++)
    {
        m_renderTargets[i].Reset();
    }
    m_depthStencilBuffer.Reset();

    // Resize the swap chain
    ThrowIfFailed(m_swapChain->ResizeBuffers(SwapChainFrameCount, m_width, m_height, BackBufferFormat,
                                             DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH));

    m_frameIndex = 0u;

    // Create/recreate frame resources.
    {
        // Create Render Targets
        // Start of the rtv heap
        CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
        for (UINT i = 0; i < SwapChainFrameCount; i++)
        {
            ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));
            m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHeapHandle);
            rtvHeapHandle.Offset(1, m_rtvDescriptorSize);

            // Create command allocator for every back buffer
            ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
                                                           IID_PPV_ARGS(&m_commandAllocators[i])));
        }

        // Create the depth/stencil view.
        D3D12_RESOURCE_DESC depthStencilDesc = {};
        depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        depthStencilDesc.Alignment = 0;
        depthStencilDesc.Width = m_width;
        depthStencilDesc.Height = m_height;
        depthStencilDesc.DepthOrArraySize = 1;
        depthStencilDesc.MipLevels = 1;
        depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS; //
        // MSAA, same settings as back buffer
        depthStencilDesc.SampleDesc.Count = m_is4xMsaaState ? 4u : 1u;
        depthStencilDesc.SampleDesc.Quality = m_is4xMsaaState ? (m_4xMsaaQuality - 1u) : 0u;

        depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

        D3D12_CLEAR_VALUE optClear = {};
        optClear.Format = DepthStencilFormat;
        optClear.DepthStencil.Depth = 1.0f;
        optClear.DepthStencil.Stencil = 0u;

        auto heapProp =
            CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT /* Once created and never changed (from CPU) */);
        ThrowIfFailed(m_device->CreateCommittedResource(&heapProp, D3D12_HEAP_FLAG_NONE, &depthStencilDesc,
                                                        D3D12_RESOURCE_STATE_COMMON, &optClear,
                                                        IID_PPV_ARGS(m_depthStencilBuffer.GetAddressOf())));

        CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());

        D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
        dsvDesc.Format = DepthStencilFormat;
        dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
        dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
        dsvDesc.Texture2D.MipSlice = 0u;

        m_device->CreateDepthStencilView(m_depthStencilBuffer.Get(), &dsvDesc, dsvHandle);
    }

    // Transition the resource from its initial state to be used as a depth buffer.
    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON,
                                                           D3D12_RESOURCE_STATE_DEPTH_WRITE);
    m_commandList->ResourceBarrier(1, &transition);

    // Execute the resize commands.
    ThrowIfFailed(m_commandList->Close());
    ID3D12CommandList *cmdLists[] = {m_commandList.Get()};
    m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

    // Wait until resize is complete.
    // FlushCommandQueue();

    m_viewport.TopLeftX = 0.0f;
    m_viewport.TopLeftY = 0.0f;
    m_viewport.Width = static_cast<FLOAT>(m_width);
    m_viewport.Height = static_cast<FLOAT>(m_height);
    m_viewport.MinDepth = 0.0f;
    m_viewport.MaxDepth = 1.0f;

    m_scissorRect.left = 0L;
    m_scissorRect.top = 0L;
    m_scissorRect.right = static_cast<LONG>(m_width);
    m_scissorRect.bottom = static_cast<LONG>(m_height);
}

void Blainn::RenderSubsystem::OnResize(UINT newWidth, UINT newHeight)
{
    // To recreate resources demanding width and height (shadow maps, G buffer etc.)
    // m_renderer->OnResize(newWidth, newHeight);
}

VOID Blainn::RenderSubsystem::WaitForGPU()
{
    // Schedule a Signal command in the queue.
    ThrowIfFailed(m_commandQueue->Signal(m_fence.Get(), m_fenceValues[m_frameIndex]));

    // Wait until the fence has been processed.
    ThrowIfFailed(m_fence->SetEventOnCompletion(m_fenceValues[m_frameIndex], m_fenceEvent));
    WaitForSingleObjectEx(m_fenceEvent, INFINITE, FALSE);

    m_fenceValues[m_frameIndex]++;
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


void Blainn::RenderSubsystem::LoadPipeline()
{
    CreateRootSignature();
    CreatePipelineStateObjects();
}

void Blainn::RenderSubsystem::CreateRootSignature()
{
}

void Blainn::RenderSubsystem::CreatePipelineStateObjects()
{
}

void Blainn::RenderSubsystem::CreateShaders()
{
}
void Blainn::RenderSubsystem::CreateAttachRenderComponent(Entity entity)
{
    RenderComponent *renderComponentPtr = entity.TryGetComponent<RenderComponent>();
    if (renderComponentPtr)
    {
        BF_ERROR("entity alrady has render component");
        return;
    }

    entity.AddComponent<RenderComponent>(RenderComponent());

    MeshComponent *meshComponentPtr = entity.TryGetComponent<MeshComponent>();
    if (meshComponentPtr)
    {
        AddMeshToRenderComponent(entity, meshComponentPtr->m_meshHandle);
    }
}

void Blainn::RenderSubsystem::AddMeshToRenderComponent(Entity entity, MeshHandle meshHandle)
{
    RenderComponent *renderComponentPtr = entity.TryGetComponent<RenderComponent>();

    if (!renderComponentPtr)
    {
        BF_ERROR("Render Subsystem :: entity does not have render component");
        return;
    }

    renderComponentPtr->m_meshCanBeRendered = true;
    renderComponentPtr->m_meshHandle = meshHandle;

    eastl::vector<MeshData> meshDataVec = meshHandle.GetMesh().GetMeshes();
    for (const auto &meshData : meshDataVec)
    {
        // TODO: create index and vertex buffer for gpu: create upload buffers and set commands in command list
        // meshData.indices;
        // meshData.vertices;
    }
}