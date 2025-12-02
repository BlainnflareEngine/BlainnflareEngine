#include "pch.h"

#include "Subsystems/RenderSubsystem.h"
#include "Engine.h"

#include "VertexTypes.h"

#include "Components/MeshComponent.h"
#include "Components/RenderComponent.h" //????
#include "File-System/Model.h"
#include "Handles/Handle.h"
#include "Scene/Scene.h"

#include "Render/CommandQueue.h"
#include "Render/FreyaMath.h"
#include "Render/FreyaUtil.h"
#include "Render/Renderer.h"
#include "Render/RootSignature.h"
#include "Render/FrameResource.h"

#include "Render/PrebuiltEngineMeshes.h"
#include "Subsystems/AssetManager.h"

#include <cassert>

using namespace Blainn;

void Blainn::RenderSubsystem::Init(HWND window)
{
    if (m_isInitialized) return;
    
    SetWindowParams(window);
    
    InitializeD3D();
    LoadGraphicsFeatures();
    LoadPipeline();

    m_isInitialized = true;
    BF_INFO("RenderSubsystem::Init() called");
}

void Blainn::RenderSubsystem::SetWindowParams(HWND window)
{
    m_hWND = window;

    RECT rect;
    GetClientRect(window, &rect);

    m_width = rect.right - rect.left;
    m_height = rect.bottom - rect.top;

    m_aspectRatio = static_cast<float>(m_width) / m_height;

    BF_DEBUG("Width: {0}", m_width);
    BF_DEBUG("Height: {0}", m_height);
}

void Blainn::RenderSubsystem::Destroy()
{
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

    auto commandQueue = m_device.GetCommandQueue();
    // Has the GPU finished processing the commands of the current frame resource?
    // If not, wait until the GPU has completed commands up to this fence point.
    if (m_currFrameResource->Fence != 0 /*&& !m_commandQueue->IsFenceComplete(m_currFrameResource->Fence)*/)
    {
        commandQueue->WaitForFenceValue(m_currFrameResource->Fence);
    }

    UpdateObjectsCB(deltaTime);
    //UpdateMaterialBuffer(deltaTime);
    //UpdateLightsBuffer(deltaTime);

    UpdateShadowTransform(deltaTime);
    UpdateShadowPassCB(deltaTime);   // pass

    UpdateGeometryPassCB(deltaTime); // pass
    UpdateMainPassCB(deltaTime);     // pass
#pragma endregion UpdateStage

#pragma region RenderStage
    // Get the current frame resource command allocator
    // Command list allocators can only be reset when the associated command lists have finished execution on the GPU; apps should use fences to determine GPU execution progress.
    auto currCmdAlloc = m_currFrameResource->commandAllocator.Get();
    currCmdAlloc->Reset();
#if defined(DEBUG) || defined(_DEBUG)
    wchar_t name[32] = {};
    UINT size = sizeof(name);
    currCmdAlloc->GetPrivateData(WKPDID_D3DDebugObjectNameW, &size, name);
#endif

    auto commandList = commandQueue->GetCommandList(currCmdAlloc);
    PopulateCommandList(commandList.Get());

    commandQueue->ExecuteCommandList(commandList);
    Present();
    
    m_currFrameResource->Fence = commandQueue->Signal(); // Advance the fence value to mark commands up to this fence point.
#pragma endregion RenderStage
}

void Blainn::RenderSubsystem::PopulateCommandList(ID3D12GraphicsCommandList2 *pCommandList)
{
    pCommandList->SetGraphicsRootSignature(m_rootSignature->Get());
    
    // Access for setting and using root descriptor table
    ID3D12DescriptorHeap *descriptorHeaps[] = {m_srvHeap.Get()};
    pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    RenderDepthOnlyPass(pCommandList);
    RenderGeometryPass(pCommandList);
    RenderLightingPass(pCommandList);
    //RenderTransparencyPass(pCommandList);
}

VOID Blainn::RenderSubsystem::InitializeD3D()
{
    CreateSwapChain();
    CreateRtvAndDsvDescriptorHeaps();
    
    Reset();

    BF_INFO("D3D12 initialized!");
}

// Helper function for setting the window's title text.
void Blainn::RenderSubsystem::SetCustomWindowText(LPCWSTR text) const
{
    // std::wstring windowText = m_title + L": " + text;
    // SetWindowText(m_hWND_, windowText.c_str());
}

VOID Blainn::RenderSubsystem::CreateSwapChain()
{
    m_swapChain = Device::GetInstance().CreateSwapChain(m_hWND, BackBufferFormat);
}

// Create descriptor heaps. Descriptor heap has to be created for every GPU resource
VOID Blainn::RenderSubsystem::CreateRtvAndDsvDescriptorHeaps()
{
    // Describe and create a render target view (RTV) descriptor heap.
    ThrowIfFailed(m_device.CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, SwapChainFrameCount + (GBuffer::EGBufferLayer::MAX - 1u), m_rtvHeap));
    m_rtvDescriptorSize = m_device.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    ThrowIfFailed(m_device.CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 3u /*main DSV + csm + DSV from GBuffer*/, m_dsvHeap));
    m_dsvDescriptorSize = m_device.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
}

VOID Blainn::RenderSubsystem::Reset()
{
    auto commandQueue = m_device.GetCommandQueue();
    auto commandAllocator = commandQueue->GetDefaultCommandAllocator();
    auto commandList = commandQueue->GetDefaultCommandList();

    assert(&m_device);
    assert(m_swapChain);
    assert(commandQueue);
    assert(commandAllocator);
    assert(commandList);

    // Before making any changes
    commandQueue->Flush();

    // temp
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart());
    m_swapChain->Reset(m_width, m_height, rtvHeapHandle, m_rtvDescriptorSize);
    m_depthStencilBuffer.Reset();

    // Create the depth/stencil view.
    D3D12_RESOURCE_DESC depthStencilDesc = {};
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = (UINT64)0;
    depthStencilDesc.Width = m_width;
    depthStencilDesc.Height = m_height;
    depthStencilDesc.DepthOrArraySize = (UINT16)1;
    depthStencilDesc.MipLevels = (UINT16)1;
    depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS; //
    // MSAA, same settings as back buffer
    depthStencilDesc.SampleDesc = m_is4xMsaaState ? DXGI_SAMPLE_DESC{4u, m_4xMsaaQuality - 1u} : DXGI_SAMPLE_DESC{1u, 0u};

    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear = {};
    optClear.Format = DepthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0u;

    ThrowIfFailed(m_device.CreateCommittedResource(D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_NONE, depthStencilDesc, 
                                                    D3D12_RESOURCE_STATE_COMMON, optClear,
                                                    m_depthStencilBuffer));

    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
    m_device.CreateDepthStencilView(m_depthStencilBuffer.Get(), DepthStencilFormat, dsvHandle);
    m_depthStencilBuffer->SetName(L"DepthStencilBuffer");

    // Transition the resource from its initial state to be used as a depth buffer.
    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(m_depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    ThrowIfFailed(commandList->Reset(commandAllocator.Get(), nullptr));

    commandList->ResourceBarrier(1, &transition);

    // Execute the resize commands.
    ThrowIfFailed(commandList->Close());
    ID3D12CommandList *const ppCommandLists[] = {commandList.Get()};
    commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    // Wait until resize is complete.
    commandQueue->Flush();

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

    if (m_areGraphicsFeaturesLoaded)
        ResetGraphicsFeatures();
}

VOID Blainn::RenderSubsystem::ResetGraphicsFeatures()
{
    m_camera->Reset(75.0f, m_aspectRatio, 1.0f, 250.0f);
}

VOID Blainn::RenderSubsystem::Present()
{
    // Present the frame.
    m_swapChain->Present();
}

void Blainn::RenderSubsystem::OnResize(UINT newWidth, UINT newHeight)
{
    // To recreate resources which depend on width and height (shadow maps, G buffer etc.)
    Reset();
    // m_renderer->OnResize(newWidth, newHeight);
}

void Blainn::RenderSubsystem::LoadPipeline()
{
    auto commandQueue = m_device.GetCommandQueue();
    auto commandAllocator = commandQueue->GetDefaultCommandAllocator();
    auto commandList = commandQueue->GetDefaultCommandList();

    ThrowIfFailed(commandList->Reset(commandAllocator.Get(), nullptr));
    
    //CreateRenderItems(commandList.Get());
    CreateFrameResources();
    CreateDescriptorHeaps();
    CreateRootSignature();

    CreateShaders();
    CreatePipelineStateObjects();

    ThrowIfFailed(commandList->Close());
    ID3D12CommandList *const ppCommandLists[] = {commandList.Get()};
    commandQueue->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

    commandQueue->Flush();
}

void Blainn::RenderSubsystem::LoadGraphicsFeatures()
{
    m_camera = eastl::make_unique<Camera>();
    
    m_cascadeShadowMap = eastl::make_unique<CascadeShadowMap>(m_device.GetDevice2().Get(), 2048u, 2048u, MaxCascades);
    m_cascadeShadowMap->CreateShadowCascadeSplits(m_camera->GetNearZ(), m_camera->GetFarZ());

    m_GBuffer = eastl::make_unique<GBuffer>(m_device.GetDevice2().Get(), m_width, m_height);
    // Explicitly reset all window params dependent features

    ResetGraphicsFeatures();
    m_areGraphicsFeaturesLoaded = true;
}

void Blainn::RenderSubsystem::CreateRenderItems(ID3D12GraphicsCommandList2 *pCommandList)
{
    /*eastl::unique_ptr<Model> cubeModel = eastl::make_unique<Model>();
    auto cubeMesh = PrebuiltEngineMeshes::CreateBox(1.0f, 1.0f, 1.0f);
    cubeModel->SetMeshes(eastl::vector<MeshData<>>{cubeMesh});
    cubeModel->CreateBufferResources(pCommandList);
    cubeModel->CreateGPUBuffers(pCommandList, cubeMesh.vertices, cubeMesh.indices);
    m_meshItems.push_back(eastl::move(cubeModel));*/
}

void Blainn::RenderSubsystem::CreateFrameResources()
{
    for (int i = 0; i < gNumFrameResources; i++)
    {
        m_frameResources.push_back(eastl::make_unique<FrameResource>(m_device, static_cast<UINT>(EPassType::NumPasses),
            0u /*(UINT)m_materials.size()*/, 0u /*MaxPointLights*/));
    }
}

void Blainn::RenderSubsystem::CreateDescriptorHeaps()
{
    ThrowIfFailed(m_device.CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
                                                 1u + 5u /*+ (UINT)m_textures.size()*/, m_srvHeap));
                                             // csm + GBuffer
    m_cbvSrvUavDescriptorSize = m_device.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

    CD3DX12_CPU_DESCRIPTOR_HANDLE handle = CD3DX12_CPU_DESCRIPTOR_HANDLE(m_srvHeap->GetCPUDescriptorHandleForHeapStart());

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    ZeroMemory(&srvDesc, sizeof(srvDesc));

    /*for (auto &e : m_textures)
    {
        auto &texD3DResource = e.second->Resource;
        srvDesc.Format = texD3DResource->GetDesc().Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2D.MostDetailedMip = 0u;
        srvDesc.Texture2D.MipLevels = texD3DResource->GetDesc().MipLevels;
        srvDesc.Texture2D.PlaneSlice;
        srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;

        m_device->CreateShaderResourceView(texD3DResource.Get(), &srvDesc, handle);

        handle.Offset(1, m_cbvSrvUavDescriptorSize);
    }*/

    UINT m_cascadeShadowMapHeapIndex = 0u;

    // configuring srv for shadow maps texture2Darray in the srv heap
    srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
    srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
    srvDesc.Texture2DArray.MostDetailedMip = 0u;
    srvDesc.Texture2DArray.MipLevels = -1;
    srvDesc.Texture2DArray.FirstArraySlice = 0u;
    srvDesc.Texture2DArray.ArraySize = m_cascadeShadowMap->Get()->GetDesc().DepthOrArraySize;
    srvDesc.Texture2DArray.PlaneSlice = 0u;
    srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
    m_device.CreateShaderResourceView(nullptr, &srvDesc, handle);

    handle.Offset(1, m_cbvSrvUavDescriptorSize);

    auto srvGpuStart = m_srvHeap->GetGPUDescriptorHandleForHeapStart();
    auto srvCpuStart = m_srvHeap->GetCPUDescriptorHandleForHeapStart();
    auto dsvCpuStart = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
    auto rtvCpuStart = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

    m_cascadeShadowSrv = CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, m_cascadeShadowMapHeapIndex, m_cbvSrvUavDescriptorSize);
    m_cascadeShadowMap->CreateDescriptors(
        CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, m_cascadeShadowMapHeapIndex, m_cbvSrvUavDescriptorSize),
        m_cascadeShadowSrv,
        CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvCpuStart, 1 /*offset from main DSV*/, m_dsvDescriptorSize));

    // to offset from csm handle to next free handle
    UINT GBufferHeapIndex = ++m_cascadeShadowMapHeapIndex;
    m_GBufferTexturesSrv = CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, GBufferHeapIndex, m_cbvSrvUavDescriptorSize);

    for (auto i = 0u; i < GBuffer::EGBufferLayer::MAX; ++i)
    {
        srvDesc.Format = (i == GBuffer::EGBufferLayer::DEPTH) ? DXGI_FORMAT_R24_UNORM_X8_TYPELESS : m_GBuffer->GetBufferTextureFormat(i);
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        m_device.CreateShaderResourceView(nullptr, &srvDesc, handle);

        auto cpuDsvRtvHandle =
            (i == GBuffer::EGBufferLayer::DEPTH)
                ? CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvCpuStart, 2 /* offset from main DSV and CSM */, m_dsvDescriptorSize)
                : CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvCpuStart, SwapChainFrameCount + i, m_rtvDescriptorSize);

        m_GBuffer->SetDescriptors(
            CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, GBufferHeapIndex, m_cbvSrvUavDescriptorSize),
            CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, GBufferHeapIndex, m_cbvSrvUavDescriptorSize), cpuDsvRtvHandle,
            i);

        handle.Offset(1, m_cbvSrvUavDescriptorSize);
        ++GBufferHeapIndex;
    }

    m_GBuffer->CreateDescriptors();
}

void Blainn::RenderSubsystem::CreateRootSignature()
{
    m_rootSignature = eastl::make_shared<RootSignature>();

    CD3DX12_DESCRIPTOR_RANGE cascadeShadowSrv;
    cascadeShadowSrv.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1u, 0u);

    CD3DX12_DESCRIPTOR_RANGE texTable;
    texTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, /*number of materials / textures*/ 6u, 2u, 0u);

    CD3DX12_DESCRIPTOR_RANGE gBufferTable;
    gBufferTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, (UINT)GBuffer::EGBufferLayer::MAX, 2u, 1u);

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[ERootParameter::NumRootParameters];

    // Perfomance TIP: Order from most frequent to least frequent.
    slotRootParameter[ERootParameter::PerObjectDataCB   ].InitAsConstantBufferView(0u, 0u, D3D12_SHADER_VISIBILITY_ALL); // a root descriptor for objects' CBVs.
    slotRootParameter[ERootParameter::PerPassDataCB     ].InitAsConstantBufferView(1u, 0u, D3D12_SHADER_VISIBILITY_ALL);   // a root descriptor for Pass CBV.

    slotRootParameter[ERootParameter::MaterialDataSB    ].InitAsShaderResourceView(1u, 0u, D3D12_SHADER_VISIBILITY_ALL);  // a srv for structured buffer with materials' data
    slotRootParameter[ERootParameter::PointLightsDataSB ].InitAsShaderResourceView(0u, 1u, D3D12_SHADER_VISIBILITY_ALL); // a srv for structured buffer
    slotRootParameter[ERootParameter::SpotLightsDataSB  ].InitAsShaderResourceView(1u, 1u, D3D12_SHADER_VISIBILITY_ALL); // a srv for structured buffer

    slotRootParameter[ERootParameter::CascadedShadowMaps].InitAsDescriptorTable(1u, &cascadeShadowSrv, D3D12_SHADER_VISIBILITY_PIXEL); // a descriptor table for shadow maps array.

    slotRootParameter[ERootParameter::Textures          ].InitAsDescriptorTable(1u, &texTable, D3D12_SHADER_VISIBILITY_PIXEL); // a descriptor table for textures

    slotRootParameter[ERootParameter::GBufferTextures   ].InitAsDescriptorTable(1u, &gBufferTable, D3D12_SHADER_VISIBILITY_PIXEL); // a descriptor table for GBuffer

    m_rootSignature->Create(m_device, ARRAYSIZE(slotRootParameter), slotRootParameter, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
}

void Blainn::RenderSubsystem::CreateShaders()
{
    // auto pixelShaderPath = GetAssetFullPath(L"./PixelShader.hlsl").c_str();

    const D3D_SHADER_MACRO fogDefines[] = {"FOG", "1", NULL, NULL};

    const D3D_SHADER_MACRO alphaTestDefines[] = {"ALPHA_TEST", "1", "FOG", "1", NULL, NULL};

    const D3D_SHADER_MACRO shadowDebugDefines[] = {"SHADOW_DEBUG", "1", NULL, NULL};

    m_shaders[EShaderType::CascadedShadowsVS]       = FreyaUtil::CompileShader(L"./Content/Shaders/ShadowVS.hlsl", nullptr, "main", "vs_5_1");
    m_shaders[EShaderType::CascadedShadowsGS]       = FreyaUtil::CompileShader(L"./Content/Shaders/CascadesGS.hlsl", nullptr, "main", "gs_5_1");

#pragma region DeferredShading
    m_shaders[EShaderType::DeferredGeometryVS]      = FreyaUtil::CompileShader(L"./Content/Shaders/GBufferPassVS.hlsl", nullptr, "main", "vs_5_1");
    m_shaders[EShaderType::DeferredGeometryPS]      = FreyaUtil::CompileShader(L"./Content/Shaders/GBufferPassPS.hlsl", nullptr, "main", "ps_5_1");
    
    m_shaders[EShaderType::DeferredDirVS]           = FreyaUtil::CompileShader(L"./Content/Shaders/DeferredDirectionalLightVS.hlsl", nullptr, "main", "vs_5_1");
    m_shaders[EShaderType::DeferredDirPS]           = FreyaUtil::CompileShader(L"./Content/Shaders/DeferredDirectionalLightPS.hlsl", nullptr, "main", "ps_5_1");

    m_shaders[EShaderType::DeferredLightVolumesVS]  = FreyaUtil::CompileShader(L"./Content/Shaders/LightVolumesVS.hlsl", nullptr, "main", "vs_5_1");
    m_shaders[EShaderType::DeferredPointPS]         = FreyaUtil::CompileShader(L"./Content/Shaders/DeferredPointLightPS.hlsl", nullptr, "main", "ps_5_1");
    m_shaders[EShaderType::DeferredSpotPS]          = FreyaUtil::CompileShader(L"./Content/Shaders/DeferredSpotLightPS.hlsl", nullptr, "main", "ps_5_1");
#pragma endregion DeferredShading
}

void Blainn::RenderSubsystem::CreatePipelineStateObjects()
{
    // To hold common properties
    D3D12_GRAPHICS_PIPELINE_STATE_DESC defaultPsoDesc = {};
    defaultPsoDesc.pRootSignature = m_rootSignature->Get();
    defaultPsoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // Blend state is disable
    defaultPsoDesc.SampleMask = UINT_MAX;
    defaultPsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    
    defaultPsoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
    defaultPsoDesc.InputLayout = BlainnVertex::InputLayout;
    defaultPsoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    defaultPsoDesc.NumRenderTargets = 1u;
    defaultPsoDesc.RTVFormats[0] = BackBufferFormat;
    defaultPsoDesc.DSVFormat = DepthStencilFormat;
    defaultPsoDesc.SampleDesc = { 1u, 0u }; // No MSAA. This should match the setting of the render target we are using (check swapChainDesc)

#pragma region CascadeShadowsDepthPass
    D3D12_GRAPHICS_PIPELINE_STATE_DESC cascadeShadowPsoDesc = defaultPsoDesc;
    
    cascadeShadowPsoDesc.VS = D3D12_SHADER_BYTECODE(
        {
            reinterpret_cast<BYTE *>(m_shaders.at(EShaderType::CascadedShadowsVS)->GetBufferPointer()),
            m_shaders.at(EShaderType::CascadedShadowsVS)->GetBufferSize()
        });
    cascadeShadowPsoDesc.GS = D3D12_SHADER_BYTECODE(
        {
            reinterpret_cast<BYTE *>(m_shaders.at(EShaderType::CascadedShadowsGS)->GetBufferPointer()),
            m_shaders.at(EShaderType::CascadedShadowsGS)->GetBufferSize()
        });

    cascadeShadowPsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    cascadeShadowPsoDesc.RasterizerState.DepthBias = 10000;
    cascadeShadowPsoDesc.RasterizerState.DepthClipEnable = (BOOL)0.0f;
    cascadeShadowPsoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
    cascadeShadowPsoDesc.InputLayout = SimpleVertex::InputLayout;
    cascadeShadowPsoDesc.NumRenderTargets = 0u;
    cascadeShadowPsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
    ThrowIfFailed(m_device.CreateGraphicsPipelineState(cascadeShadowPsoDesc, m_pipelineStates[EPsoType::CascadedShadowsOpaque]));
#pragma endregion CascadeShadowsDepthPass

#pragma region DeferredShading
    D3D12_GRAPHICS_PIPELINE_STATE_DESC GBufferPsoDesc = defaultPsoDesc;

    GBufferPsoDesc.VS = D3D12_SHADER_BYTECODE(
        {
            reinterpret_cast<BYTE *>(m_shaders.at(EShaderType::DeferredGeometryVS)->GetBufferPointer()),
            m_shaders.at(EShaderType::DeferredGeometryVS)->GetBufferSize()
        });
    GBufferPsoDesc.PS = D3D12_SHADER_BYTECODE(
        {reinterpret_cast<BYTE *>(m_shaders.at(EShaderType::DeferredGeometryPS)->GetBufferPointer()),
         m_shaders.at(EShaderType::DeferredGeometryPS)->GetBufferSize()});

    GBufferPsoDesc.NumRenderTargets = static_cast<UINT>(GBuffer::EGBufferLayer::MAX) - 1u;
    GBufferPsoDesc.RTVFormats[0] = m_GBuffer->GetBufferTextureFormat(GBuffer::EGBufferLayer::DIFFUSE_ALBEDO);
    GBufferPsoDesc.RTVFormats[1] = m_GBuffer->GetBufferTextureFormat(GBuffer::EGBufferLayer::AMBIENT_OCCLUSION);
    GBufferPsoDesc.RTVFormats[2] = m_GBuffer->GetBufferTextureFormat(GBuffer::EGBufferLayer::NORMAL);
    GBufferPsoDesc.RTVFormats[3] = m_GBuffer->GetBufferTextureFormat(GBuffer::EGBufferLayer::SPECULAR);
    ThrowIfFailed(m_device.CreateGraphicsPipelineState(GBufferPsoDesc, m_pipelineStates[EPsoType::DeferredGeometry]));

    // not sure it works
#pragma region Wireframe
    D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframe = GBufferPsoDesc;
    opaqueWireframe.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    ThrowIfFailed(m_device.CreateGraphicsPipelineState(opaqueWireframe, m_pipelineStates[EPsoType::Wireframe]));
#pragma endregion Wireframe

#pragma region DeferredDirectional
    D3D12_GRAPHICS_PIPELINE_STATE_DESC dirLightPsoDesc = defaultPsoDesc;
    dirLightPsoDesc.VS = D3D12_SHADER_BYTECODE(
        {
            reinterpret_cast<BYTE *>(m_shaders.at(EShaderType::DeferredDirVS)->GetBufferPointer()),
            m_shaders.at(EShaderType::DeferredDirVS)->GetBufferSize()
        });
    dirLightPsoDesc.PS = D3D12_SHADER_BYTECODE(
        {
            reinterpret_cast<BYTE *>(m_shaders.at(EShaderType::DeferredDirPS)->GetBufferPointer()),
            m_shaders.at(EShaderType::DeferredDirPS)->GetBufferSize()
        });
    dirLightPsoDesc.InputLayout = SimpleVertex::InputLayout;
    ThrowIfFailed(m_device.CreateGraphicsPipelineState(dirLightPsoDesc, m_pipelineStates[EPsoType::DeferredDirectional]));
#pragma endregion DeferredDirectional

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

    pointLightIntersectsFarPlanePsoDesc.VS = D3D12_SHADER_BYTECODE(
        {
            reinterpret_cast<BYTE *>(m_shaders.at(EShaderType::DeferredLightVolumesVS)->GetBufferPointer()),
            m_shaders.at(EShaderType::DeferredLightVolumesVS)->GetBufferSize()
        });
    pointLightIntersectsFarPlanePsoDesc.PS = D3D12_SHADER_BYTECODE(
        {
                reinterpret_cast<BYTE *>(m_shaders.at(EShaderType::DeferredPointPS)->GetBufferPointer()),
                m_shaders.at(EShaderType::DeferredPointPS)->GetBufferSize()
        });
    pointLightIntersectsFarPlanePsoDesc.BlendState.AlphaToCoverageEnable = FALSE;
    pointLightIntersectsFarPlanePsoDesc.BlendState.IndependentBlendEnable = FALSE;
    pointLightIntersectsFarPlanePsoDesc.BlendState.RenderTarget[0] = RTBlendDesc;

    pointLightIntersectsFarPlanePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.DepthEnable = FALSE;
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.StencilEnable = FALSE;
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.StencilReadMask = 0xFF;
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.StencilWriteMask = 0xFF;
    ThrowIfFailed(m_device.CreateGraphicsPipelineState(pointLightIntersectsFarPlanePsoDesc, m_pipelineStates[EPsoType::DeferredPointIntersectsFarPlane]));
    

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pointLightWithinFrustumPsoDesc = pointLightIntersectsFarPlanePsoDesc;
    pointLightIntersectsFarPlanePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // ???
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
    ThrowIfFailed(m_device.CreateGraphicsPipelineState(pointLightWithinFrustumPsoDesc, m_pipelineStates[EPsoType::DeferredPointWithinFrustum]));

#pragma endregion DeferredPointLight

#pragma region DeferredSpotLight
    D3D12_GRAPHICS_PIPELINE_STATE_DESC spotLightPsoDesc = pointLightIntersectsFarPlanePsoDesc;
    spotLightPsoDesc.PS = D3D12_SHADER_BYTECODE(
        {
            reinterpret_cast<BYTE *>(m_shaders.at(EShaderType::DeferredSpotPS)->GetBufferPointer()),
            m_shaders.at(EShaderType::DeferredSpotPS)->GetBufferSize()
        });
    ThrowIfFailed(m_device.CreateGraphicsPipelineState(spotLightPsoDesc, m_pipelineStates[EPsoType::DeferredSpot]));
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
        AddMeshToRenderComponent(entity, *meshComponentPtr->m_meshHandle);
    }
}

// TODO: make MeshHandle shared_ptr
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

    eastl::vector<MeshData<>> meshDataVec = meshHandle.GetMesh().GetMeshes();
    for (const auto &meshData : meshDataVec)
    {
        // TODO: create index and vertex buffer for gpu: create upload buffers and set commands in command list
        // meshData.indices;
        // meshData.vertices;
    }
}

void Blainn::RenderSubsystem::OnKeyboardInput(float deltaTime)
{
}

void Blainn::RenderSubsystem::UpdateObjectsCB(float deltaTime)
{
    const auto &renderEntitiesView = Engine::GetActiveScene()->GetAllEntitiesWith<IDComponent, TransformComponent, MeshComponent>();
    for (const auto& [entity, entityID, entityTransform, entityMesh] : renderEntitiesView.each())
    {
        // Luna stuff. Try to remove 'if' statement.
        // Have tried. It does not affect anything.
        // Looks like it just forces the code to update the object's constant buffer regardless of whether it has been modified or not.
        if (entityTransform.IsDirty())
        {
            ObjectConstants objConstants;

            auto world = entityTransform.GetTransform();
            auto transposeWorld = world.Transpose();
            auto invTransposeWorld = transposeWorld.Invert();
            
            XMStoreFloat4x4(&objConstants.World, transposeWorld);
            XMStoreFloat4x4(&objConstants.InvTransposeWorld, XMMatrixTranspose(invTransposeWorld));
            // XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(ri->TexTransform));
            // objConstants.MaterialIndex = ri->Mat->MatBufferIndex;

            entityMesh.UpdateMeshCB(objConstants);
            //ri->NumFramesDirty--;
        }
    }
}

void Blainn::RenderSubsystem::UpdateMaterialBuffer(float deltaTime)
{
    /*auto currMaterialDataSB = m_currFrameResource->MaterialSB.get();

    for (auto &e : m_materials)
    {
        Material *mat = e.second.get();
        if (mat->NumFramesDirty > 0)
        {
            m_perMaterialSBData.DiffuseAlbedo = mat->DiffuseAlbedo;
            m_perMaterialSBData.FresnelR0 = mat->FresnelR0;
            m_perMaterialSBData.Roughness = mat->Roughness;
            XMStoreFloat4x4(&m_perMaterialSBData.MatTransform, XMMatrixTranspose(mat->MatTransform));
            m_perMaterialSBData.DiffusseMapIndex = mat->DiffuseSrvHeapIndex;

            currMaterialDataSB->CopyData(mat->MatBufferIndex, m_perMaterialSBData);

            mat->NumFramesDirty--;
        }
    }*/
}

void Blainn::RenderSubsystem::UpdateLightsBuffer(float deltaTime)
{
    //auto currPointLightSB = m_currFrameResource->PointLightSB.get();

    //for (auto &e : m_pointLights)
    //{
    //    // we have many instances, not the one objects, so think about it (we can't update all instances, if only one
    //    // point light gets dirty)
    //    // if (e->NumFramesDirty > 0)
    //    //{

    //    int pointLightIndex = 0;
    //    const auto &instances = e->Instances;

    //    for (UINT i = 0; i < (UINT)instances.size(); ++i)
    //    {
    //        XMStoreFloat4x4(&m_perInstanceSBData.World, XMMatrixTranspose(XMLoadFloat4x4(&instances[i].World)));
    //        m_perInstanceSBData.Light.Strength = instances[i].Light.Strength;
    //        m_perInstanceSBData.Light.FallOfStart = instances[i].Light.FallOfStart;
    //        m_perInstanceSBData.Light.FallOfEnd = instances[i].Light.FallOfEnd;
    //        m_perInstanceSBData.Light.Position = instances[i].Light.Position;
    //        // copy all instances to structured buffer
    //        currPointLightSB->CopyData(pointLightIndex++, m_perInstanceSBData);
    //    }
    //    e->InstanceCount = pointLightIndex;

    //    // e->NumFramesDirty--;
    //    // }
    //}
}

void Blainn::RenderSubsystem::UpdateShadowTransform(float deltaTime)
{
    eastl::vector<eastl::pair<XMMATRIX, XMMATRIX>> lightSpaceMatrices;
    GetLightSpaceMatrices(lightSpaceMatrices);

    for (UINT i = 0; i < MaxCascades; ++i)
    {
        XMMATRIX shadowTransform = lightSpaceMatrices[i].first * lightSpaceMatrices[i].second;
        m_shadowPassCBData.Cascades.CascadeViewProj[i] = XMMatrixTranspose(shadowTransform);

        m_mainPassCBData.Cascades.CascadeViewProj[i] = XMMatrixTranspose(shadowTransform);
        m_mainPassCBData.Cascades.Distances[i] = m_cascadeShadowMap->GetCascadeLevel(i);
    }
}

void Blainn::RenderSubsystem::UpdateShadowPassCB(float deltaTime)
{
    XMMATRIX view = XMMatrixIdentity();
    XMMATRIX proj = XMMatrixIdentity();
    XMMATRIX viewProj = XMMatrixMultiply(view, proj);

    auto det = XMMatrixDeterminant(viewProj);
    XMMATRIX invViewProj = XMMatrixInverse(&det, viewProj);

    XMStoreFloat4x4(&m_shadowPassCBData.View, XMMatrixTranspose(view));
    XMStoreFloat4x4(&m_shadowPassCBData.Proj, XMMatrixTranspose(proj));
    XMStoreFloat4x4(&m_shadowPassCBData.ViewProj, XMMatrixTranspose(viewProj));
    XMStoreFloat4x4(&m_shadowPassCBData.InvViewProj, XMMatrixTranspose(invViewProj));

    auto currPassCB = m_currFrameResource->PassCB.get();
    currPassCB->CopyData(static_cast<int>(EPassType::DepthShadow), m_shadowPassCBData);
}

void Blainn::RenderSubsystem::UpdateGeometryPassCB(float deltaTime)
{
    XMMATRIX view = m_camera->GetViewMatrix();
    XMMATRIX proj = m_camera->GetPerspectiveProjectionMatrix();
    XMMATRIX viewProj = XMMatrixMultiply(view, proj);
    auto det = XMMatrixDeterminant(viewProj);
    XMMATRIX invViewProj = XMMatrixInverse(&det, viewProj);

    XMStoreFloat4x4(&m_geometryPassCBData.View, XMMatrixTranspose(view));
    XMStoreFloat4x4(&m_geometryPassCBData.Proj, XMMatrixTranspose(proj));
    XMStoreFloat4x4(&m_geometryPassCBData.ViewProj, XMMatrixTranspose(viewProj));
    XMStoreFloat4x4(&m_geometryPassCBData.InvViewProj, XMMatrixTranspose(invViewProj));

    m_geometryPassCBData.EyePosW = m_camera->GetPosition3f();
    m_geometryPassCBData.RenderTargetSize = XMFLOAT2((float)m_width, (float)m_height);
    m_geometryPassCBData.InvRenderTargetSize = XMFLOAT2(1.0f / m_width, 1.0f / m_height);
    m_geometryPassCBData.NearZ = m_camera->GetNearZ();
    m_geometryPassCBData.FarZ = m_camera->GetFarZ();
    m_geometryPassCBData.DeltaTime = deltaTime;
    m_geometryPassCBData.TotalTime = deltaTime;

    auto currPassCB = m_currFrameResource->PassCB.get();
    currPassCB->CopyData(static_cast<int>(EPassType::DeferredGeometry), m_geometryPassCBData);
}

void Blainn::RenderSubsystem::UpdateMainPassCB(float deltaTime)
{
    XMMATRIX view = m_camera->GetViewMatrix();
    XMMATRIX proj = m_camera->GetPerspectiveProjectionMatrix();
    XMMATRIX viewProj = XMMatrixMultiply(view, proj);
    auto det = XMMatrixDeterminant(viewProj);
    XMMATRIX invViewProj = XMMatrixInverse(&det, viewProj);

    XMStoreFloat4x4(&m_mainPassCBData.View, XMMatrixTranspose(view));
    XMStoreFloat4x4(&m_mainPassCBData.Proj, XMMatrixTranspose(proj));
    XMStoreFloat4x4(&m_mainPassCBData.ViewProj, XMMatrixTranspose(viewProj));
    XMStoreFloat4x4(&m_mainPassCBData.InvViewProj, XMMatrixTranspose(invViewProj));

    m_mainPassCBData.EyePosW = m_camera->GetPosition3f();
    m_mainPassCBData.RenderTargetSize = XMFLOAT2((float)m_width, (float)m_height);
    m_mainPassCBData.InvRenderTargetSize = XMFLOAT2(1.0f / m_width, 1.0f / m_height);
    m_mainPassCBData.NearZ = m_camera->GetNearZ();
    m_mainPassCBData.FarZ = m_camera->GetFarZ();
    m_mainPassCBData.DeltaTime = deltaTime;
    m_mainPassCBData.TotalTime = deltaTime;

    m_mainPassCBData.Ambient = {0.25f, 0.25f, 0.35f, 1.0f};

#pragma region DirLight
    // Invert sign because other way light would be pointing up
    XMVECTOR lightDir = -FreyaMath::SphericalToCarthesian(1.0f, m_sunTheta, m_sunPhi);
    XMStoreFloat3(&m_mainPassCBData.DirLight.Direction, lightDir);
    m_mainPassCBData.DirLight.Strength = {1.0f, 1.0f, 0.9f};
#pragma endregion DirLight

    auto currPassCB = m_currFrameResource->PassCB.get();
    currPassCB->CopyData(static_cast<int>(EPassType::DeferredLighting), m_mainPassCBData);
}

void Blainn::RenderSubsystem::RenderDepthOnlyPass(ID3D12GraphicsCommandList2 *pCommandList)
{
    UINT passCBByteSize = FreyaUtil::CalcConstantBufferByteSize(sizeof(PassConstants));

    auto csmViewport = m_cascadeShadowMap->GetViewport();
    auto csmScissor = m_cascadeShadowMap->GetScissorRect();

    pCommandList->RSSetViewports(1u, &csmViewport);
    pCommandList->RSSetScissorRects(1u, &csmScissor);

    // change to depth write state
    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(m_cascadeShadowMap->Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    pCommandList->ResourceBarrier(1u, &transition);

#pragma region BypassResources
    auto currFramePassCB = m_currFrameResource->PassCB->Get();
    auto currFrameGPUVirtualAddress = FreyaUtil::GetGPUVirtualAddress(currFramePassCB->GetGPUVirtualAddress(), passCBByteSize, static_cast<UINT>(EPassType::DepthShadow));
    pCommandList->SetGraphicsRootConstantBufferView(ERootParameter::PerPassDataCB, currFrameGPUVirtualAddress);
#pragma endregion BypassResources

    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_cascadeShadowMap->GetDsv());
    pCommandList->OMSetRenderTargets(0u, nullptr, TRUE, &dsvHandle);
    pCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0u, 0u, nullptr);

    pCommandList->SetPipelineState(m_pipelineStates.at(EPsoType::CascadedShadowsOpaque).Get());
    DrawMeshes(pCommandList, m_meshItems);

    transition = CD3DX12_RESOURCE_BARRIER::Transition(m_cascadeShadowMap->Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
    pCommandList->ResourceBarrier(1u, &transition);
}

void Blainn::RenderSubsystem::RenderGeometryPass(ID3D12GraphicsCommandList2 *pCommandList)
{
    UINT passCBByteSize = FreyaUtil::CalcConstantBufferByteSize(sizeof(PassConstants));
    // The viewport needs to be reset whenever the command list is reset.
    pCommandList->RSSetViewports(1u, &m_viewport);
    pCommandList->RSSetScissorRects(1u, &m_scissorRect);

    for (unsigned i = 0; i < GBuffer::EGBufferLayer::MAX - 1u; i++)
    {
        auto transition = CD3DX12_RESOURCE_BARRIER::Transition(m_GBuffer->Get(i), D3D12_RESOURCE_STATE_GENERIC_READ,
                                                               D3D12_RESOURCE_STATE_RENDER_TARGET);
        pCommandList->ResourceBarrier(1u, &transition);
    }
    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(m_GBuffer->Get(GBuffer::EGBufferLayer::DEPTH), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);
    pCommandList->ResourceBarrier(1u, &transition);

#pragma region BypassResources
    auto currFramePassCB = m_currFrameResource->PassCB->Get();
    auto currFramePassCBAddress = FreyaUtil::GetGPUVirtualAddress(currFramePassCB->GetGPUVirtualAddress(), passCBByteSize, static_cast<UINT>(EPassType::DeferredGeometry));
    pCommandList->SetGraphicsRootConstantBufferView(ERootParameter::PerPassDataCB, currFramePassCBAddress); // second element contains data for geometry pass

    // Bind all the materials used in this scene. For structured buffers, we can bypass the heap and set as a root
    // descriptor.
    //auto matBuffer = m_currFrameResource->MaterialSB->Get();
    //pCommandList->SetGraphicsRootShaderResourceView(ERootParameter::MaterialDataSB, matBuffer->GetGPUVirtualAddress());

    // Bind all the textures used in this scene. Observe that we only have to specify the first descriptor in the table.
    // The root signature knows how many descriptors are expected in the table.
    // pCommandList->SetGraphicsRootDescriptorTable(ERootParameter::Textures, m_srvHeap->GetGPUDescriptorHandleForHeapStart());
#pragma endregion BypassResources

    // start of the GBuffer rtvs in rtvHeap
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), SwapChainFrameCount,
                                            m_rtvDescriptorSize);

    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_GBuffer->GetDsv(GBuffer::EGBufferLayer::DEPTH));
    pCommandList->OMSetRenderTargets(GBuffer::EGBufferLayer::DEPTH, &rtvHandle, TRUE, &dsvHandle);

    const float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    pCommandList->ClearRenderTargetView(m_GBuffer->GetRtv(GBuffer::EGBufferLayer::DIFFUSE_ALBEDO),
                                        Colors::LightSteelBlue, 0u, nullptr);
    pCommandList->ClearRenderTargetView(m_GBuffer->GetRtv(GBuffer::EGBufferLayer::AMBIENT_OCCLUSION), clearColor, 0u,
                                        nullptr);
    pCommandList->ClearRenderTargetView(m_GBuffer->GetRtv(GBuffer::EGBufferLayer::NORMAL), clearColor, 0u, nullptr);
    pCommandList->ClearRenderTargetView(m_GBuffer->GetRtv(GBuffer::EGBufferLayer::SPECULAR), clearColor, 0u, nullptr);
    pCommandList->ClearDepthStencilView(m_GBuffer->GetDsv(GBuffer::EGBufferLayer::DEPTH),
                                        D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0u, 0u, nullptr);

    pCommandList->SetPipelineState(m_pipelineStates.at(EPsoType::DeferredGeometry).Get());
    
    DrawMeshes(pCommandList, m_meshItems);

    for (unsigned i = 0; i < GBuffer::EGBufferLayer::MAX - 1u; i++)
    {
        auto transition = CD3DX12_RESOURCE_BARRIER::Transition(m_GBuffer->Get(i), D3D12_RESOURCE_STATE_RENDER_TARGET,
                                                               D3D12_RESOURCE_STATE_GENERIC_READ);
        pCommandList->ResourceBarrier(1u, &transition);
    }

    transition = CD3DX12_RESOURCE_BARRIER::Transition(m_GBuffer->Get(GBuffer::EGBufferLayer::DEPTH),
                                             D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
    pCommandList->ResourceBarrier(1u, &transition);
}

void Blainn::RenderSubsystem::RenderLightingPass(ID3D12GraphicsCommandList2 *pCommandList)
{
    DeferredDirectionalLightPass(pCommandList);
    DeferredPointLightPass(pCommandList);
    //DeferredSpotLightPass(pCommandList);
}

void Blainn::RenderSubsystem::DeferredDirectionalLightPass(ID3D12GraphicsCommandList2 *pCommandList)
{
    UINT passCBByteSize = FreyaUtil::CalcConstantBufferByteSize(sizeof(PassConstants));

    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(m_swapChain->GetBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    // Indicate that the back buffer will be used as a render target.
    pCommandList->ResourceBarrier(1u, &transition);

    // The viewport needs to be reset whenever the command list is reset.
    pCommandList->RSSetViewports(1u, &m_viewport);
    pCommandList->RSSetScissorRects(1u, &m_scissorRect);

    auto rtvHandle = GetRTV();
    auto dsvHandle = GetDSV();

    const float *clearColor = &m_mainPassCBData.FogColor.x;
    pCommandList->OMSetRenderTargets(1u, &rtvHandle, TRUE, &dsvHandle);
    pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0u, nullptr);
    pCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0u, 0u, nullptr);

#pragma region BypassResources
    auto currFramePassCB = m_currFrameResource->PassCB->Get();
    auto currFramePassCBAddress = FreyaUtil::GetGPUVirtualAddress(currFramePassCB->GetGPUVirtualAddress(), passCBByteSize, static_cast<UINT>(EPassType::DeferredLighting));
    
    pCommandList->SetGraphicsRootConstantBufferView(ERootParameter::PerPassDataCB, currFramePassCBAddress);
    // Set shaadow map texture for main pass
    pCommandList->SetGraphicsRootDescriptorTable(ERootParameter::CascadedShadowMaps, m_cascadeShadowSrv);

    // Bind GBuffer textures
    pCommandList->SetGraphicsRootDescriptorTable(ERootParameter::GBufferTextures, m_GBufferTexturesSrv);
#pragma endregion BypassResources

    pCommandList->SetPipelineState(m_pipelineStates.at(EPsoType::DeferredDirectional).Get());
    DrawQuad(pCommandList);
}

void Blainn::RenderSubsystem::DeferredPointLightPass(ID3D12GraphicsCommandList2 *pCommandList)
{
    UINT passCBByteSize = FreyaUtil::CalcConstantBufferByteSize(sizeof(PassConstants));

    auto currFramePassCB = m_currFrameResource->PassCB->Get();
    auto currFramePassCBAddress = FreyaUtil::GetGPUVirtualAddress(currFramePassCB->GetGPUVirtualAddress(), passCBByteSize, static_cast<UINT>(EPassType::DeferredLighting));
    //pCommandList->SetGraphicsRootConstantBufferView(ERootParameter::PerPassDataCB, currFramePassCBAddress);

    // Bind GBuffer textures
    //pCommandList->SetGraphicsRootDescriptorTable(ERootParameter::GBufferTextures, m_GBufferTexturesSrv);

    // !!! HACK (TO DRAW EVEN IF FRUSTUM INTERSECTS LIGHT VOLUME)
    //pCommandList->SetPipelineState(m_pipelineStates.at(EPsoType::DeferredPointWithinFrustum).Get());
    
    //DrawInstancedMeshes(m_commandList, m_pointLights);
    //DrawInstancedRenderItems(pCommandList, m_pointLights);

    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(m_swapChain->GetBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    // Indicate that the back buffer will now be used to present.
    pCommandList->ResourceBarrier(1u, &transition);
}

void Blainn::RenderSubsystem::DeferredSpotLightPass(ID3D12GraphicsCommandList2 *pCommandList)
{
}

void Blainn::RenderSubsystem::RenderTransparencyPass(ID3D12GraphicsCommandList2 *pCommandList)
{
}

void Blainn::RenderSubsystem::DrawMeshes(ID3D12GraphicsCommandList2 *pCommandList, const eastl::vector<eastl::unique_ptr<Model>>& models)
{
    UINT objCBByteSize = (UINT)FreyaUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

    const auto& renderEntitiesView = Engine::GetActiveScene()->GetAllEntitiesWith<IDComponent, TransformComponent, MeshComponent>();
    for (const auto& [entity, entityID, entityTransform, entityMesh] : renderEntitiesView.each())
    {
        //auto& model = entityMesh.m_meshHandle->GetMesh();
        auto currObjectCB = entityMesh.ObjectCB->Get();

        auto &model = AssetManager::GetInstance().GetMeshByIndex(0);
        
        auto currVBV = model.VertexBufferView();
        auto currIBV = model.IndexBufferView();

        pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        pCommandList->IASetVertexBuffers(0u, 1u, &currVBV);
        pCommandList->IASetIndexBuffer(&currIBV);

        D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = FreyaUtil::GetGPUVirtualAddress(currObjectCB->GetGPUVirtualAddress(), objCBByteSize, 0u);
        pCommandList->SetGraphicsRootConstantBufferView(ERootParameter::PerObjectDataCB, objCBAddress);

        if (currVBV.SizeInBytes)
        {
            pCommandList->DrawIndexedInstanced(model.GetIndicesCount(), 1u, 0u, 0u, 0u);
        }
        else
        {
            pCommandList->DrawInstanced(model.GetVerticesCount(), 1u, 0u, 0u);
        }
    }
}

void Blainn::RenderSubsystem::DrawQuad(ID3D12GraphicsCommandList2 *pCommandList)
{
    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    pCommandList->DrawInstanced(4u, 1u, 0u, 0u);
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
    float minX = eastl::numeric_limits<float>::max();
    float minY = eastl::numeric_limits<float>::max();
    float minZ = eastl::numeric_limits<float>::max();
    float maxX = eastl::numeric_limits<float>::lowest();
    float maxY = eastl::numeric_limits<float>::lowest();
    float maxZ = eastl::numeric_limits<float>::lowest();

    for (const auto &v : frustumCorners)
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

    return eastl::make_pair(lightView, lightProj);
}

void Blainn::RenderSubsystem::GetLightSpaceMatrices(eastl::vector<eastl::pair<XMMATRIX, XMMATRIX>> &outMatrices)
{
    for (UINT i = 0; i < MaxCascades; ++i)
    {
        if (i == 0)
        {
            outMatrices.push_back(GetLightSpaceMatrix(m_camera->GetNearZ(), m_cascadeShadowMap->GetCascadeLevel(i)));
        }
        else
        {
            outMatrices.push_back(GetLightSpaceMatrix(m_cascadeShadowMap->GetCascadeLevel(i-1), m_cascadeShadowMap->GetCascadeLevel(i)));
        }
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