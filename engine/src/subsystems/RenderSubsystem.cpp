#include "pch.h"

#include "subsystems/RenderSubsystem.h"

#include <cassert>
#include <iostream>

#include "Render/FreyaMath.h"
#include "Render/FreyaUtil.h"
#include "Render/Renderer.h"
#include "VertexTypes.h"

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

    m_aspectRatio = static_cast<float>(m_width) / m_height;

    BF_DEBUG("Width: {0}", m_width);
    BF_DEBUG("Height: {0}", m_height);

#if defined(_DEBUG)
    // Enable the debug layer (requires the Graphics Tools "optional feature").
    // NOTE: Enabling the debug layer after device creation will invalidate the active device.
    CreateDebugLayer();
#endif
    m_camera = eastl::make_unique<Camera>();

    InitializeD3D();

    m_renderer = eastl::make_unique<Renderer>(m_device.Get(), m_width, m_height);
    m_renderer->Init();

    LoadPipeline();

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
    // BF_INFO("RenderSubsystem::Render()");

#pragma region UpdateStage
    m_camera->Update(deltaTime);

    // Cycle through the circular frame resource array.
    m_currFrameResourceIndex = (m_currFrameResourceIndex + 1) % gNumFrameResources;
    m_currFrameResource = m_frameResources[m_currFrameResourceIndex].get();

    // UpdateObjectsCB(deltaTime);
    // UpdateMaterialBuffer(deltaTime);
    // UpdateLightsBuffer(deltaTime);

    // UpdateShadowTransform(deltaTime);
    // UpdateShadowPassCB(deltaTime);   // pass

    // UpdateGeometryPassCB(deltaTime); // pass
    // UpdateMainPassCB(deltaTime);     // pass
#pragma endregion UpdateStage

#pragma region RenderStage
    // Record all the commands we need to render the scene into the command list.
    // m_renderer->PopulateCommandList();
    PopulateCommandList();

    eastl::shared_ptr<Scene> scene = Engine::GetActiveScene();
    auto renderedEntities = scene->GetAllEntitiesWith<IDComponent, RenderComponent>();
    for (auto entityComponents : renderedEntities.each())
    {
        IDComponent &idComponent = std::get<1>(entityComponents);
        RenderComponent &renderComponent = std::get<2>(entityComponents);

        if (!renderComponent.m_visible || !renderComponent.m_meshCanBeRendered) continue;

        // TODO: render component
    }

    // m_renderer->ExecuteCommandLists();
    ID3D12CommandList *ppCommandLists[] = {m_commandList.Get()};
    m_commandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Present the frame.
    ThrowIfFailed(m_swapChain->Present(1u, 0u));
#pragma endregion RenderStage

    MoveToNextFrame();
}

void Blainn::RenderSubsystem::PopulateCommandList()
{
    auto currentCmdAlloc = m_currFrameResource->commandAllocator.Get();

    ThrowIfFailed(currentCmdAlloc->Reset());

    ThrowIfFailed(m_commandList->Reset(currentCmdAlloc, m_pipelineStates.at(EPsoType::DeferredGeometry).Get()));

    m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

    // Access for setting and using root descriptor table
    ID3D12DescriptorHeap *descriptorHeaps[] = {m_srvHeap.Get()};
    m_commandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    RenderDepthOnlyPass();
    /*RenderGeometryPass();
    RenderLightingPass();
    RenderTransparencyPass();*/

    ThrowIfFailed(m_commandList->Close());
}

VOID Blainn::RenderSubsystem::InitializeD3D()
{
    CreateDevice();
    CreateCommandObjects();
    CreateFence();
    CreateRtvAndDsvDescriptorHeaps();
    CreateSwapChain();

    Reset();

    BF_INFO("D3D initialized!");
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
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
    for (UINT i = 0; i < SwapChainFrameCount; i++)
    {
        ThrowIfFailed(m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_renderTargets[i])));
        m_device->CreateRenderTargetView(m_renderTargets[i].Get(), nullptr, rtvHeapHandle);
        rtvHeapHandle.Offset(1, m_rtvDescriptorSize);

        // Create command allocator for every back buffer
        ThrowIfFailed(
            m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocators[i])));
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

    auto heapProp = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT /* Once created and never changed (from CPU) */);
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

    m_camera->Reset(75.0f, m_aspectRatio, 1.0f, 250.0f);

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
    ThrowIfFailed(m_commandList->Reset(m_commandAllocators[m_frameIndex].Get(), nullptr));

    CreateFrameResources();
    CreateDescriptorHeaps();
    CreateRootSignature();

    CreateShaders();
    CreatePipelineStateObjects();

    m_commandList->Close();
    ID3D12CommandList *cmdLists[] = {m_commandList.Get()};
    m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);
}

void Blainn::RenderSubsystem::CreateFrameResources()
{
    for (int i = 0; i < gNumFrameResources; i++)
    {
        m_frameResources.push_back(eastl::make_unique<FrameResource>(
            m_device.Get(), static_cast<UINT>(EPassType::NumPasses), 0u /*(UINT)m_renderItems.size()*/,
            0u /*(UINT)m_materials.size()*/, 0u /*MaxPointLights*/));
    }
}

void Blainn::RenderSubsystem::CreateDescriptorHeaps()
{
    D3D12_DESCRIPTOR_HEAP_DESC srvHeapDesc = {};
    ZeroMemory(&srvHeapDesc, sizeof(srvHeapDesc));
    srvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
    // textures + csm + GBuffer
    srvHeapDesc.NumDescriptors = /*(UINT)m_textures.size()*/ +1u + 5u;
    srvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
    srvHeapDesc.NodeMask = 0u;
    ThrowIfFailed(m_device->CreateDescriptorHeap(&srvHeapDesc, IID_PPV_ARGS(m_srvHeap.GetAddressOf())));

    m_cbvSrvUavDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void Blainn::RenderSubsystem::CreateRootSignature()
{
    CD3DX12_DESCRIPTOR_RANGE cascadeShadowSrv;
    cascadeShadowSrv.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1u, 0u);

    CD3DX12_DESCRIPTOR_RANGE texTable;
    texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
                  /*magic test number (supposed to be the number of materials/textures*/ 6u, 2u, 0u);

    CD3DX12_DESCRIPTOR_RANGE gBufferTable;
    gBufferTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, (UINT)GBuffer::EGBufferLayer::MAX, 2u, 1u);

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[ERootParameter::NumRootParameters];

    // Perfomance TIP: Order from most frequent to least frequent.
    slotRootParameter[ERootParameter::PerObjectDataCB].InitAsConstantBufferView(
        0u, 0u,
        D3D12_SHADER_VISIBILITY_ALL /* gMaterialIndex used in both shaders */); // a root descriptor for objects' CBVs.
    slotRootParameter[ERootParameter::PerPassDataCB].InitAsConstantBufferView(
        1u, 0u, D3D12_SHADER_VISIBILITY_ALL); // a root descriptor for Pass CBV.
    slotRootParameter[ERootParameter::MaterialDataSB].InitAsShaderResourceView(
        1u, 0u, D3D12_SHADER_VISIBILITY_ALL /* gMaterialData used in both shaders */); // a srv for structured buffer
                                                                                       // with materials' data
    slotRootParameter[ERootParameter::PointLightsDataSB].InitAsShaderResourceView(
        0u, 1u, D3D12_SHADER_VISIBILITY_ALL /* gMaterialData used in both shaders */); // a srv for structured buffer
    slotRootParameter[ERootParameter::SpotLightsDataSB].InitAsShaderResourceView(
        1u, 1u, D3D12_SHADER_VISIBILITY_ALL /* gMaterialData used in both shaders */); // a srv for structured buffer
    slotRootParameter[ERootParameter::CascadedShadowMaps].InitAsDescriptorTable(
        1u, &cascadeShadowSrv, D3D12_SHADER_VISIBILITY_PIXEL); // a descriptor table for shadow maps array.
    slotRootParameter[ERootParameter::Textures].InitAsDescriptorTable(
        1u, &texTable, D3D12_SHADER_VISIBILITY_PIXEL); // a descriptor table for textures
    slotRootParameter[ERootParameter::GBufferTextures].InitAsDescriptorTable(
        1u, &gBufferTable, D3D12_SHADER_VISIBILITY_PIXEL); // a descriptor table for GBuffer

    auto staticSamplers = GetStaticSamplers();

    // Root signature is an array of root parameters
    CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
    rootSignatureDesc.Init(ARRAYSIZE(slotRootParameter), slotRootParameter, (UINT)staticSamplers.size(),
                           staticSamplers.data(), D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

    ComPtr<ID3DBlob> signature;
    ComPtr<ID3DBlob> error;
    ThrowIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
    ThrowIfFailed(m_device->CreateRootSignature(0u, signature->GetBufferPointer(), signature->GetBufferSize(),
                                                IID_PPV_ARGS(&m_rootSignature)));
}

void Blainn::RenderSubsystem::CreateShaders()
{
    // auto pixelShaderPath = GetAssetFullPath(L"./PixelShader.hlsl").c_str();

    const D3D_SHADER_MACRO fogDefines[] = {"FOG", "1", NULL, NULL};

    const D3D_SHADER_MACRO alphaTestDefines[] = {"ALPHA_TEST", "1", "FOG", "1", NULL, NULL};

    const D3D_SHADER_MACRO shadowDebugDefines[] = {"SHADOW_DEBUG", "1", NULL, NULL};

    m_shaders[EShaderType::CascadedShadowsVS] =
        FreyaUtil::CompileShader(L"./Content/Shaders/ShadowVS.hlsl", nullptr, "main", "vs_5_1");
    m_shaders[EShaderType::CascadedShadowsGS] =
        FreyaUtil::CompileShader(L"./Content/Shaders/CascadesGS.hlsl", nullptr, "main", "gs_5_1");

#pragma region DeferredShading
    m_shaders[EShaderType::DeferredGeometryVS] =
        FreyaUtil::CompileShader(L"./Content/Shaders/GBufferPassVS.hlsl", nullptr, "main", "vs_5_1");
    m_shaders[EShaderType::DeferredGeometryPS] =
        FreyaUtil::CompileShader(L"./Content/Shaders/GBufferPassPS.hlsl", nullptr, "main", "ps_5_1");

    m_shaders[EShaderType::DeferredDirVS] =
        FreyaUtil::CompileShader(L"./Content/Shaders/DeferredDirectionalLightVS.hlsl", nullptr, "main", "vs_5_1");
    m_shaders[EShaderType::DeferredDirPS] =
        FreyaUtil::CompileShader(L"./Content/Shaders/DeferredDirectionalLightPS.hlsl", nullptr, "main", "ps_5_1");

    m_shaders[EShaderType::DeferredLightVolumesVS] =
        FreyaUtil::CompileShader(L"./Content/Shaders/LightVolumesVS.hlsl", nullptr, "main", "vs_5_1");
    m_shaders[EShaderType::DeferredPointPS] =
        FreyaUtil::CompileShader(L"./Content/Shaders/DeferredPointLightPS.hlsl", nullptr, "main", "ps_5_1");
    m_shaders[EShaderType::DeferredSpotPS] =
        FreyaUtil::CompileShader(L"./Content/Shaders/DeferredSpotLightPS.hlsl", nullptr, "main", "ps_5_1");
#pragma endregion DeferredShading
}

void Blainn::RenderSubsystem::CreatePipelineStateObjects()
{
#pragma region DefaultOpaqueAndWireframe
    // Describe and create the graphics pipeline state object (PSO).

#pragma endregion DefaultOpaqueAndWireframe

#pragma region CascadeShadowsDepthPass
    D3D12_GRAPHICS_PIPELINE_STATE_DESC cascadeShadowPsoDesc = {};
    ZeroMemory(&cascadeShadowPsoDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
    cascadeShadowPsoDesc.pRootSignature = m_rootSignature.Get();
    cascadeShadowPsoDesc.VS = D3D12_SHADER_BYTECODE(
        {reinterpret_cast<BYTE *>(m_shaders.at(EShaderType::CascadedShadowsVS)->GetBufferPointer()),
         m_shaders.at(EShaderType::CascadedShadowsVS)->GetBufferSize()});
    cascadeShadowPsoDesc.GS = D3D12_SHADER_BYTECODE(
        {reinterpret_cast<BYTE *>(m_shaders.at(EShaderType::CascadedShadowsGS)->GetBufferPointer()),
         m_shaders.at(EShaderType::CascadedShadowsGS)->GetBufferSize()});
    cascadeShadowPsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // Blend state is disable
    cascadeShadowPsoDesc.SampleMask = UINT_MAX;
    cascadeShadowPsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    cascadeShadowPsoDesc.RasterizerState.DepthBias = 10000;
    cascadeShadowPsoDesc.RasterizerState.DepthClipEnable = (BOOL)0.0f;
    cascadeShadowPsoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
    cascadeShadowPsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    cascadeShadowPsoDesc.InputLayout = VertexPosition::InputLayout;
    cascadeShadowPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    cascadeShadowPsoDesc.NumRenderTargets = 0u;
    cascadeShadowPsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
    cascadeShadowPsoDesc.DSVFormat = DepthStencilFormat;
    cascadeShadowPsoDesc.SampleDesc.Count = 1u;
    cascadeShadowPsoDesc.SampleDesc.Quality = 0u;
    ThrowIfFailed(m_device->CreateGraphicsPipelineState(
        &cascadeShadowPsoDesc, IID_PPV_ARGS(&m_pipelineStates[EPsoType::CascadedShadowsOpaque])));
#pragma endregion CascadeShadowsDepthPass

#pragma region DeferredShading

    D3D12_GRAPHICS_PIPELINE_STATE_DESC GBufferPsoDesc = {};
    GBufferPsoDesc.pRootSignature = m_rootSignature.Get();

    GBufferPsoDesc.VS = D3D12_SHADER_BYTECODE(
        {reinterpret_cast<BYTE *>(m_shaders.at(EShaderType::DeferredGeometryVS)->GetBufferPointer()),
         m_shaders.at(EShaderType::DeferredGeometryVS)->GetBufferSize()});
    GBufferPsoDesc.PS = D3D12_SHADER_BYTECODE(
        {reinterpret_cast<BYTE *>(m_shaders.at(EShaderType::DeferredGeometryPS)->GetBufferPointer()),
         m_shaders.at(EShaderType::DeferredGeometryPS)->GetBufferSize()});

    GBufferPsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // Blend state is disable
    GBufferPsoDesc.SampleMask = UINT_MAX;
    GBufferPsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    GBufferPsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    GBufferPsoDesc.InputLayout = VertexPositionNormalTangentBitangentUV::InputLayout;
    GBufferPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    GBufferPsoDesc.NumRenderTargets = static_cast<UINT>(GBuffer::EGBufferLayer::MAX) - 1u;
    GBufferPsoDesc.RTVFormats[0] = m_GBuffer->GetBufferTextureFormat(GBuffer::EGBufferLayer::DIFFUSE_ALBEDO);
    GBufferPsoDesc.RTVFormats[1] = m_GBuffer->GetBufferTextureFormat(GBuffer::EGBufferLayer::AMBIENT_OCCLUSION);
    GBufferPsoDesc.RTVFormats[2] = m_GBuffer->GetBufferTextureFormat(GBuffer::EGBufferLayer::NORMAL);
    GBufferPsoDesc.RTVFormats[3] = m_GBuffer->GetBufferTextureFormat(GBuffer::EGBufferLayer::SPECULAR);
    GBufferPsoDesc.DSVFormat = DepthStencilFormat; // corresponds to default format
    GBufferPsoDesc.SampleDesc = {1u, 0u};
    ThrowIfFailed(m_device->CreateGraphicsPipelineState(&GBufferPsoDesc,
                                                        IID_PPV_ARGS(&m_pipelineStates[EPsoType::DeferredGeometry])));

    D3D12_GRAPHICS_PIPELINE_STATE_DESC dirLightPsoDesc = {};
    dirLightPsoDesc.pRootSignature = m_rootSignature.Get();
    dirLightPsoDesc.VS =
        D3D12_SHADER_BYTECODE({reinterpret_cast<BYTE *>(m_shaders.at(EShaderType::DeferredDirVS)->GetBufferPointer()),
                               m_shaders.at(EShaderType::DeferredDirVS)->GetBufferSize()});
    dirLightPsoDesc.PS =
        D3D12_SHADER_BYTECODE({reinterpret_cast<BYTE *>(m_shaders.at(EShaderType::DeferredDirPS)->GetBufferPointer()),
                               m_shaders.at(EShaderType::DeferredDirPS)->GetBufferSize()});
    dirLightPsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // Blend state is disable
    dirLightPsoDesc.SampleMask = UINT_MAX;
    dirLightPsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    dirLightPsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    dirLightPsoDesc.InputLayout = VertexPosition::InputLayout;
    dirLightPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    dirLightPsoDesc.NumRenderTargets = 1u;
    dirLightPsoDesc.RTVFormats[0] = BackBufferFormat;
    dirLightPsoDesc.DSVFormat = DepthStencilFormat;
    dirLightPsoDesc.SampleDesc = {1u, 0u};
    ThrowIfFailed(m_device->CreateGraphicsPipelineState(
        &dirLightPsoDesc, IID_PPV_ARGS(&m_pipelineStates[EPsoType::DeferredDirectional])));

#pragma region DeferredPointLight
    // Still needs to be configured
    // Hack with DSS and RS
    D3D12_GRAPHICS_PIPELINE_STATE_DESC pointLightIntersectsFarPlanePsoDesc = dirLightPsoDesc;

    D3D12_RENDER_TARGET_BLEND_DESC RTBlendDesc = {};
    ZeroMemory(&RTBlendDesc, sizeof(D3D12_RENDER_TARGET_BLEND_DESC));
    RTBlendDesc.BlendEnable = TRUE;
    RTBlendDesc.LogicOpEnable = FALSE;
    RTBlendDesc.SrcBlend = D3D12_BLEND_ONE;
    RTBlendDesc.DestBlend = D3D12_BLEND_ONE;
    RTBlendDesc.BlendOp = D3D12_BLEND_OP_ADD;
    RTBlendDesc.SrcBlendAlpha = D3D12_BLEND_ONE;
    RTBlendDesc.DestBlendAlpha = D3D12_BLEND_ONE;
    RTBlendDesc.BlendOpAlpha = D3D12_BLEND_OP_ADD;
    RTBlendDesc.LogicOp = D3D12_LOGIC_OP_NOOP;
    RTBlendDesc.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

    pointLightIntersectsFarPlanePsoDesc.BlendState.AlphaToCoverageEnable = FALSE;
    pointLightIntersectsFarPlanePsoDesc.BlendState.IndependentBlendEnable = FALSE;
    pointLightIntersectsFarPlanePsoDesc.BlendState.RenderTarget[0] = RTBlendDesc;

    pointLightIntersectsFarPlanePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.DepthEnable = FALSE;
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.StencilEnable = FALSE;
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.StencilReadMask = 0xFF;
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.StencilWriteMask = 0xFF;
    pointLightIntersectsFarPlanePsoDesc.VS = D3D12_SHADER_BYTECODE(
        {reinterpret_cast<BYTE *>(m_shaders.at(EShaderType::DeferredLightVolumesVS)->GetBufferPointer()),
         m_shaders.at(EShaderType::DeferredLightVolumesVS)->GetBufferSize()});
    pointLightIntersectsFarPlanePsoDesc.PS =
        D3D12_SHADER_BYTECODE({reinterpret_cast<BYTE *>(m_shaders.at(EShaderType::DeferredPointPS)->GetBufferPointer()),
                               m_shaders.at(EShaderType::DeferredPointPS)->GetBufferSize()});
    ThrowIfFailed(m_device->CreateGraphicsPipelineState(
        &pointLightIntersectsFarPlanePsoDesc,
        IID_PPV_ARGS(&m_pipelineStates[EPsoType::DeferredPointIntersectsFarPlane])));

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pointLightWithinFrustumPsoDesc = pointLightIntersectsFarPlanePsoDesc;
    pointLightIntersectsFarPlanePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT; // ???
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
    pointLightIntersectsFarPlanePsoDesc.InputLayout = VertexPosition::InputLayout;
    ThrowIfFailed(m_device->CreateGraphicsPipelineState(
        &pointLightWithinFrustumPsoDesc, IID_PPV_ARGS(&m_pipelineStates[EPsoType::DeferredPointWithinFrustum])));

#pragma endregion DeferredPointLight

#pragma region DeferredSpotLight
    D3D12_GRAPHICS_PIPELINE_STATE_DESC spotLightPsoDesc = pointLightIntersectsFarPlanePsoDesc;
    spotLightPsoDesc.PS =
        D3D12_SHADER_BYTECODE({reinterpret_cast<BYTE *>(m_shaders.at(EShaderType::DeferredSpotPS)->GetBufferPointer()),
                               m_shaders.at(EShaderType::DeferredSpotPS)->GetBufferSize()});
    ThrowIfFailed(m_device->CreateGraphicsPipelineState(&spotLightPsoDesc,
                                                        IID_PPV_ARGS(&m_pipelineStates[EPsoType::DeferredSpot])));
#pragma endregion DeferredSpotLight

#pragma endregion DeferredShading
}

/*
 * Relation with Render component
 */

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

// TODO: make MeshHandle shared_ptr
void Blainn::RenderSubsystem::AddMeshToRenderComponent(Entity entity, eastl::shared_ptr<Blainn::MeshHandle> meshHandle)
{
    RenderComponent *renderComponentPtr = entity.TryGetComponent<RenderComponent>();

    if (!renderComponentPtr)
    {
        BF_ERROR("Render Subsystem :: entity does not have render component");
        return;
    }

    renderComponentPtr->m_meshCanBeRendered = true;
    renderComponentPtr->m_meshHandle = meshHandle;

    eastl::vector<MeshData> meshDataVec = meshHandle->GetMesh().GetMeshes();
    for (const auto &meshData : meshDataVec)
    {
        // TODO: create index and vertex buffer for gpu: create upload buffers and set commands in command list
        // meshData.indices;
        // meshData.vertices;
    }
}

void Blainn::RenderSubsystem::RenderDepthOnlyPass()
{
    m_commandList->RSSetViewports(1u, &m_viewport);
    m_commandList->RSSetScissorRects(1u, &m_scissorRect);

    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(
        m_renderTargets[m_frameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    m_commandList->ResourceBarrier(1u, &transition);

    auto rtvHandle = GetRTV();
    auto dsvHandle = GetDSV();

    const float *clearColor = DirectX::Colors::Yellow;
    m_commandList->OMSetRenderTargets(1u, &rtvHandle, TRUE, &dsvHandle);
    m_commandList->ClearRenderTargetView(rtvHandle, clearColor, 0u, nullptr);
    m_commandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0u, 0u,
                                         nullptr);

    m_commandList->SetPipelineState(m_pipelineStates.at(EPsoType::CascadedShadowsOpaque).Get());

    transition = CD3DX12_RESOURCE_BARRIER::Transition(m_renderTargets[m_frameIndex].Get(),
                                                      D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    m_commandList->ResourceBarrier(1u, &transition);
}

void Blainn::RenderSubsystem::RenderGeometryPass()
{
}

void Blainn::RenderSubsystem::RenderLightingPass()
{
}

void Blainn::RenderSubsystem::RenderTransparencyPass()
{
}

void Blainn::RenderSubsystem::DeferredDirectionalLightPass()
{
}

void Blainn::RenderSubsystem::DeferredPointLightPass()
{
}

void Blainn::RenderSubsystem::DeferredSpotLightPass()
{
}

void Blainn::RenderSubsystem::DrawQuad(ID3D12GraphicsCommandList *cmdList)
{
}

eastl::array<const CD3DX12_STATIC_SAMPLER_DESC, 5> Blainn::RenderSubsystem::GetStaticSamplers()
{
    const CD3DX12_STATIC_SAMPLER_DESC pointWrap(0u,                               // shaderRegister
                                                D3D12_FILTER_MIN_MAG_MIP_POINT,   // filter
                                                D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
                                                D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
                                                D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

    const CD3DX12_STATIC_SAMPLER_DESC linearWrap(1u,                               // shaderRegister
                                                 D3D12_FILTER_MIN_MAG_MIP_LINEAR,  // filter
                                                 D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressU
                                                 D3D12_TEXTURE_ADDRESS_MODE_WRAP,  // addressV
                                                 D3D12_TEXTURE_ADDRESS_MODE_WRAP); // addressW

    const CD3DX12_STATIC_SAMPLER_DESC anisotropicWrap(2u,                              // shaderRegister
                                                      D3D12_FILTER_ANISOTROPIC,        // filter
                                                      D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressU
                                                      D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressV
                                                      D3D12_TEXTURE_ADDRESS_MODE_WRAP, // addressW
                                                      0.0f,                            // mipLODBias
                                                      8u);                             // maxAnisotropy

    const CD3DX12_STATIC_SAMPLER_DESC shadowSampler(
        3u, D3D12_FILTER_MIN_MAG_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0.0f, 16u);

    const CD3DX12_STATIC_SAMPLER_DESC shadowComparison(
        4u, D3D12_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR, D3D12_TEXTURE_ADDRESS_MODE_BORDER,
        D3D12_TEXTURE_ADDRESS_MODE_BORDER, D3D12_TEXTURE_ADDRESS_MODE_BORDER, 0.0f, 16u,
        D3D12_COMPARISON_FUNC_LESS_EQUAL, D3D12_STATIC_BORDER_COLOR_OPAQUE_BLACK);

    return {pointWrap, linearWrap, anisotropicWrap, shadowSampler, shadowComparison};
}

eastl::pair<XMMATRIX, XMMATRIX> Blainn::RenderSubsystem::GetLightSpaceMatrix(const float nearZ, const float farZ)
{
    const auto directionalLight = m_mainPassCBData.DirLight;

    const XMFLOAT3 lightDir = directionalLight.Direction;

    const auto cameraProj = XMMatrixPerspectiveFovLH(m_camera->GetFovYRad(), m_aspectRatio, nearZ, farZ);
    const auto frustumCorners = GetFrustumCornersWorldSpace(m_camera->GetViewMatrix(), cameraProj);

    XMVECTOR center = XMVectorZero();
    for (const auto &v : frustumCorners)
    {
        center += v;
    }
    center /= (float)frustumCorners.size();

    const XMMATRIX lightView =
        XMMatrixLookAtLH(center, center + XMVectorSet(lightDir.x, lightDir.y, lightDir.z, 1.0f), FreyaMath::UpVector);

    // Measuring cascade
    float minX = std::numeric_limits<float>::max();
    float minY = std::numeric_limits<float>::max();
    float minZ = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::lowest();
    float maxY = std::numeric_limits<float>::lowest();
    float maxZ = std::numeric_limits<float>::lowest();

    for (const auto &v : frustumCorners)
    {
        const auto trf = XMVector4Transform(v, lightView);

        minX = std::min(minX, XMVectorGetX(trf));
        maxX = std::max(maxX, XMVectorGetX(trf));
        minY = std::min(minY, XMVectorGetY(trf));
        maxY = std::max(maxY, XMVectorGetY(trf));
        minZ = std::min(minZ, XMVectorGetZ(trf));
        maxZ = std::max(maxZ, XMVectorGetZ(trf));
    }
    // Tune this parameter according to the scene
    constexpr float zMult = 10.0f;
    minZ = (minZ < 0) ? minZ * zMult : minZ / zMult;
    maxZ = (maxZ < 0) ? maxZ / zMult : maxZ * zMult;

    const XMMATRIX lightProj = XMMatrixOrthographicOffCenterLH(minX, maxX, minY, maxY, minZ, maxZ);

    return eastl::make_pair(lightView, lightProj);
}

void Blainn::RenderSubsystem::GetLightSpaceMatrices(eastl::vector<eastl::pair<XMMATRIX, XMMATRIX>> &outMatrices)
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

eastl::vector<XMVECTOR> Blainn::RenderSubsystem::GetFrustumCornersWorldSpace(const XMMATRIX &view,
                                                                             const XMMATRIX &projection)
{
    const auto viewProj = view * projection;

    XMVECTOR det = XMMatrixDeterminant(viewProj);
    const auto invViewProj = XMMatrixInverse(&det, viewProj);

    eastl::vector<XMVECTOR> frustumCorners;
    frustumCorners.reserve(8);

    for (UINT x = 0; x < 2; ++x)
    {
        for (UINT y = 0; y < 2; ++y)
        {
            for (UINT z = 0; z < 2; ++z)
            {
                // translate NDC coords to world space
                const XMVECTOR pt =
                    XMVector4Transform(XMVectorSet(2.0f * x - 1.0f, 2.0f * y - 1.0f, (float)z, 1.0f), invViewProj);
                frustumCorners.push_back(pt / XMVectorGetW(pt));
            }
        }
    }
    return frustumCorners;
}