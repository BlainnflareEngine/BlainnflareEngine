#include "pch.h"

#include "Subsystems/RenderSubsystem.h"
#include "Subsystems/AssetManager.h"
#include "Engine.h"

#include "VertexTypes.h"

#include "Components/MeshComponent.h"
#include "File-System/Model.h"
#include "File-System/Material.h"
#include "Handles/Handle.h"
#include "Scene/Scene.h"

#include "Render/CommandQueue.h"
#include "Render/DebugRenderer.h"
#include "Render/FrameResource.h"
#include "Render/FreyaMath.h"
#include "Render/FreyaUtil.h"
#include "Render/PrebuiltEngineMeshes.h"
#include "Render/RootSignature.h"
#include "Render/Shader.h"
#include "Render/EditorCamera.h"
#include "Render/RuntimeCamera.h"
#include "Render/DDSTextureLoader.h"
#include "Render/UI/UIRenderer.h"
#include "Render/GTexture.h"

#include "PhysicsSubsystem.h"
#include "Components/PhysicsComponent.h"
#include "Components/LightComponent.h"

#include <cassert>

namespace Blainn
{
void Blainn::RenderSubsystem::PreInit()
{
    CreateDescriptorHeaps();
}

void Blainn::RenderSubsystem::Init(HWND window)
{
    if (m_isInitialized) return;

    SetWindowParams(window);

    InitializeWindow();
    LoadGraphicsFeatures();
    LoadPipeline();

    m_debugRenderer = eastl::make_unique<Blainn::DebugRenderer>(m_device);
    m_UIRenderer = eastl::make_unique<Blainn::UIRenderer>();

    m_UIRenderer->Initialize(m_width, m_height);

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
    m_UIRenderer = nullptr;
    m_debugRenderer = nullptr;

    m_isInitialized = false;
    BF_INFO("RenderSubsystem::Destroy()");
}

void RenderSubsystem::SetEnableDebug(bool newValue)
{
    m_enableDebugLayer = newValue;
    if (m_debugRenderer)
    {
        m_debugRenderer->SetDebugEnabled(newValue);
        m_debugRenderer->ClearDebugList();
    }
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
    UpdateLightsBuffers(deltaTime);
    UpdateMaterialBuffer(deltaTime);

    UpdateShadowTransform(deltaTime);
    UpdateShadowPassCB(deltaTime); // pass

    UpdateGeometryPassCB(deltaTime); // pass
    UpdateDeferredPassCB(deltaTime); // pass
    //UpdateForwardPassCB(deltaTime); // pass
#pragma endregion UpdateStage

#pragma region RenderStage
    // Get the current frame resource command allocator
    // Command list allocators can only be reset when the associated command lists have finished execution on the GPU;
    // apps should use fences to determine GPU execution progress.
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

    m_currFrameResource->Fence =
        commandQueue->Signal(); // Advance the fence value to mark commands up to this fence point.
#pragma endregion RenderStage

    commandQueue->Flush();
}

uuid RenderSubsystem::GetUUIDAt(uint32_t x, uint32_t y)
{
    BLAINN_PROFILE_FUNC();
    if (m_UIRenderer->IsUIHovered())
        return Engine::GetSelectionManager().GetSelectedUUID();
    if (x > m_width || y > m_height)
        return uuid{0, 0};

    auto device = m_device.GetDevice2();
    m_device.Flush();

    auto copyQueue = m_device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_COPY);
    auto directQueue = m_device.GetCommandQueue(D3D12_COMMAND_LIST_TYPE_DIRECT);

    ComPtr<ID3D12CommandAllocator> copyAllocator;
    m_device.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, copyAllocator);

    ComPtr<ID3D12CommandAllocator> directAllocator;
    m_device.CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, directAllocator);

    ComPtr<ID3D12GraphicsCommandList2> copyList = copyQueue->GetCommandList(copyAllocator.Get());
    ComPtr<ID3D12GraphicsCommandList2> directList = directQueue->GetCommandList(directAllocator.Get());

    RenderUUIDPass(directList.Get());

    directQueue->ExecuteCommandList(directList.Get());
    m_device.Flush();
    auto texture = m_uuidRenderTarget.GetTexture(AttachmentPoint::Color0);

    auto texDesc = texture->GetD3D12ResourceDesc();
    auto textureRes = texture->GetD3D12Resource();

    D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
    UINT numRows;
    UINT64 rowSizeInBytes;
    UINT64 totalSize = 0;

    device->GetCopyableFootprints(&texDesc, 0, 1, 0, &footprint, &numRows, &rowSizeInBytes, &totalSize);

    if (totalSize == 0)
    {
        BF_ERROR("Failed to create copyable footprint");
        return uuid{0, 0};
    }

    D3D12_RESOURCE_DESC buffersDesc = CD3DX12_RESOURCE_DESC::Buffer(totalSize);
    D3D12_HEAP_PROPERTIES const heapPropertiesReadback = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_READBACK);

    ComPtr<ID3D12Resource> textureReadback = nullptr;
    if (FAILED(device->CreateCommittedResource(&heapPropertiesReadback, D3D12_HEAP_FLAG_NONE, &buffersDesc,
                                    D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&textureReadback))))
    {
        BF_ERROR("Failed to create readback buffer");
        return uuid{0, 0};
    }

    m_device.Flush();

    D3D12_TEXTURE_COPY_LOCATION src{};
    src.pResource = textureRes.Get();
    src.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
    src.SubresourceIndex = 0;

    D3D12_TEXTURE_COPY_LOCATION dst{};
    dst.pResource = textureReadback.Get();
    dst.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
    dst.PlacedFootprint = footprint;

    copyList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

    copyQueue->ExecuteCommandList(copyList.Get());
    m_device.Flush();

    uint8_t *data;
    textureReadback->Map(0, nullptr, reinterpret_cast<void **>(&data));

    uint8_t *rowStart = data + y * footprint.Footprint.RowPitch;
    uint8_t *texel = rowStart + x * 16;
    uuid id(texel);

    textureReadback->Unmap(0, nullptr);
    return id;
}

void Blainn::RenderSubsystem::PopulateCommandList(ID3D12GraphicsCommandList2 *pCommandList)
{
    BLAINN_PROFILE_FUNC();
    pCommandList->SetGraphicsRootSignature(m_rootSignature->Get());

    // Access for setting and using root descriptor table
    ID3D12DescriptorHeap *descriptorHeaps[] = {m_device.GetDescriptorHeap().Get()};
    pCommandList->SetDescriptorHeaps(_countof(descriptorHeaps), descriptorHeaps);

    RenderDepthOnlyPass(pCommandList);
    RenderGeometryPass(pCommandList);
    RenderLightingPass(pCommandList);
    RenderForwardPasses(pCommandList);

    if (m_enableDebugLayer) RenderDebugPass(pCommandList);

    RenderImGuiPass(pCommandList);
}

VOID Blainn::RenderSubsystem::InitializeWindow()
{
    CreateSwapChain();
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
VOID Blainn::RenderSubsystem::CreateDescriptorHeaps()
{
    // Describe and create a render target view (RTV) descriptor heap.
    ThrowIfFailed(m_device.CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
                                                SwapChainFrameCount + (GBuffer::EGBufferLayer::MAX - 1u) + 10));
    ThrowIfFailed(m_device.CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 3u + 10));
    ThrowIfFailed(m_device.CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 2048u + 1000,
                                                D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE));

    // Cache descriptor heaps
    m_rtvHeap = m_device.GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_dsvHeap = m_device.GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_srvHeap = m_device.GetDescriptorHeap();

    // Cache descriptor heap increment sizes
    m_rtvDescriptorSize = m_device.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    m_dsvDescriptorSize = m_device.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    m_cbvSrvUavDescriptorSize = m_device.GetDescriptorHandleIncrementSize();
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
    ThrowIfFailed(commandList->Reset(commandAllocator.Get(), nullptr));

    m_swapChain->Reset(m_width, m_height);
    m_depthStencilBuffer.Reset();

    // Create the depth/stencil view.
    D3D12_RESOURCE_DESC depthStencilDesc = {};
    depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
    depthStencilDesc.Alignment = (UINT64)0;
    depthStencilDesc.Width = m_width;
    depthStencilDesc.Height = m_height;
    depthStencilDesc.DepthOrArraySize = (UINT16)1;
    depthStencilDesc.MipLevels = (UINT16)1;
    depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS; // MSAA, same settings as back buffer
    depthStencilDesc.SampleDesc =
        m_is4xMsaaState ? DXGI_SAMPLE_DESC{4u, m_4xMsaaQuality - 1u} : DXGI_SAMPLE_DESC{1u, 0u};

    depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
    depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

    D3D12_CLEAR_VALUE optClear = {};
    optClear.Format = DepthStencilFormat;
    optClear.DepthStencil.Depth = 1.0f;
    optClear.DepthStencil.Stencil = 0u;

    ThrowIfFailed(m_device.CreateCommittedResource(D3D12_HEAP_TYPE_DEFAULT, D3D12_HEAP_FLAG_NONE, depthStencilDesc,
                                                   D3D12_RESOURCE_STATE_COMMON, optClear, m_depthStencilBuffer));

    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart());
    m_device.CreateDepthStencilView(m_depthStencilBuffer.Get(), DepthStencilFormat, dsvHandle);
    m_depthStencilBuffer->SetName(L"DepthStencilBuffer");

    // Transition the resource from its initial state to be used as a depth buffer.
    ResourceBarrier(commandList.Get(), m_depthStencilBuffer.Get(), D3D12_RESOURCE_STATE_COMMON,
                    D3D12_RESOURCE_STATE_DEPTH_WRITE);

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

    if (m_areGraphicsFeaturesLoaded) ResetGraphicsFeatures();
}

VOID Blainn::RenderSubsystem::ResetGraphicsFeatures()
{
    // m_camera->Reset(75.0f, m_aspectRatio, 0.1f, 250.0f);
    m_camera->SetAspectRatio(m_aspectRatio);

    m_GBuffer->OnResize(m_width, m_height);
    m_cascadeShadowMap->OnResize(2048u, 2048u);

    m_uuidRenderTarget.Resize(m_width, m_height);
}

VOID Blainn::RenderSubsystem::Present()
{
    // Present the frame.
    m_swapChain->Present();
}

void Blainn::RenderSubsystem::OnResize(UINT newWidth, UINT newHeight)
{
    if (!m_isInitialized) return;

    m_width = newWidth;
    m_height = newHeight;
    m_aspectRatio = static_cast<float>(m_width) / m_height;

    m_UIRenderer->Resize(m_width, m_height);

    // To recreate resources which depend on width and height (shadow maps, GBuffer etc.)
    Reset();
}

void Blainn::RenderSubsystem::LoadPipeline()
{
    auto commandQueue = m_device.GetCommandQueue();
    auto commandAllocator = commandQueue->GetDefaultCommandAllocator();
    auto commandList = commandQueue->GetDefaultCommandList();

    ThrowIfFailed(commandList->Reset(commandAllocator.Get(), nullptr));

    CreateFrameResources();
    LoadInitTimeTextures(commandList.Get());
    LoadSrvAndSamplerDescriptorHeaps();
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
    m_editorCamera = eastl::make_shared<EditorCamera>();
    // m_camera = m_editorCamera.;
    m_camera = m_editorCamera.get();

    m_cascadeShadowMap = eastl::make_unique<CascadeShadowMap>(m_device.GetDevice2().Get(), 2048u, 2048u, MaxCascades);
    m_cascadeShadowMap->CreateShadowCascadeSplits(m_camera->GetNearZ(), m_camera->GetFarZ());

    m_GBuffer = eastl::make_unique<GBuffer>(m_device.GetDevice2().Get(), m_width, m_height);

    skyBox = eastl::make_unique<MeshComponent>(AssetManager::GetDefaultMesh());

    auto uuidTexDesc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R32G32B32A32_UINT, m_width, m_height, 1, 0, 1, 0,
                                                    D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);
    D3D12_CLEAR_VALUE optClearValue = {};
    optClearValue.Format = uuidTexDesc.Format;
    optClearValue.Color[0] = 0.f;
    optClearValue.Color[1] = 0.f;
    optClearValue.Color[2] = 0.f;
    optClearValue.Color[3] = 0.f;

    eastl::shared_ptr<GTexture> uuidTexture = eastl::make_shared<GTexture>(m_device, uuidTexDesc, &optClearValue);
    uuidTexture->SetName(L"UUID Render Target");
    m_uuidRenderTarget.AttachTexture(AttachmentPoint::Color0, std::move(uuidTexture));

    // Explicitly reset all window params dependent features
    // ResetGraphicsFeatures();
    m_camera->Reset(75.0f, m_aspectRatio, 0.1f, 250.0f);
    m_areGraphicsFeaturesLoaded = true;
}

void Blainn::RenderSubsystem::CreateFrameResources()
{
    for (int i = 0; i < gNumFrameResources; i++)
    {
        m_frameResources.push_back(eastl::make_unique<FrameResource>(m_device, static_cast<UINT>(EPassType::NumPasses), MAX_MATERIALS, MaxPointLights, MaxSpotLights));
    }
}

void Blainn::RenderSubsystem::LoadInitTimeTextures(ID3D12GraphicsCommandList2 *pCommandList)
{
    ThrowIfFailed(CreateDDSTextureFromFile12(m_device.GetDevice2().Get(), pCommandList,
                                             L"./Content/Textures/sunsetcube1024.dds", skyBoxResource,
                                             skyBoxUploadHeap));
}

void Blainn::RenderSubsystem::LoadSrvAndSamplerDescriptorHeaps()
{
    auto srvGpuStart = m_srvHeap->GetGPUDescriptorHandleForHeapStart();
    auto srvCpuStart = m_srvHeap->GetCPUDescriptorHandleForHeapStart();
    auto dsvCpuStart = m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
    auto rtvCpuStart = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();

    //!!!!!! Set imgui Gizmos stuff at zero index in srvHeap

    D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
    ZeroMemory(&srvDesc, sizeof(srvDesc));

    m_cascadesShadowSrvHeapStartIndex = 0u;
    CD3DX12_CPU_DESCRIPTOR_HANDLE localHandle =
        CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, m_cascadesShadowSrvHeapStartIndex, m_cbvSrvUavDescriptorSize);
    {
        // configuring srv for shadow maps texture2Darray in the srv heap
        srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.Texture2DArray.MostDetailedMip = 0u;
        srvDesc.Texture2DArray.MipLevels = -1;
        srvDesc.Texture2DArray.FirstArraySlice = 0u;
        srvDesc.Texture2DArray.ArraySize = m_cascadeShadowMap->Get()->GetDesc().DepthOrArraySize;
        srvDesc.Texture2DArray.PlaneSlice = 0u;
        srvDesc.Texture2DArray.ResourceMinLODClamp = 0.0f;
        m_device.CreateShaderResourceView(nullptr, &srvDesc, localHandle); // set shadow srv to first element of srvHeap

        m_cascadeShadowMap->CreateDescriptors(
            CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, m_cascadesShadowSrvHeapStartIndex, m_cbvSrvUavDescriptorSize),
            CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, m_cascadesShadowSrvHeapStartIndex, m_cbvSrvUavDescriptorSize),
            CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvCpuStart, 1,
                                          m_device.GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV)));
    }

    m_GBufferTexturesSrvHeapStartIndex = m_cascadesShadowSrvHeapStartIndex + 1u;
    localHandle =
        CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, m_GBufferTexturesSrvHeapStartIndex, m_cbvSrvUavDescriptorSize);
    for (auto i = 0u; i < GBuffer::EGBufferLayer::MAX; ++i)
    {
        srvDesc.Format = (i == GBuffer::EGBufferLayer::DEPTH) ? DXGI_FORMAT_R24_UNORM_X8_TYPELESS
                                                              : m_GBuffer->GetBufferTextureFormat(i);
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        m_device.CreateShaderResourceView(nullptr, &srvDesc, localHandle);

        auto cpuDsvRtvHandle =
            (i == GBuffer::EGBufferLayer::DEPTH)
                ? CD3DX12_CPU_DESCRIPTOR_HANDLE(dsvCpuStart, 2, m_dsvDescriptorSize)
                : CD3DX12_CPU_DESCRIPTOR_HANDLE(rtvCpuStart, SwapChainFrameCount + i, m_rtvDescriptorSize);

        m_GBuffer->SetDescriptors(CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, m_GBufferTexturesSrvHeapStartIndex + i,
                                                                m_cbvSrvUavDescriptorSize),
                                  CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, m_GBufferTexturesSrvHeapStartIndex + i,
                                                                m_cbvSrvUavDescriptorSize),
                                  cpuDsvRtvHandle, i);
        localHandle.Offset(1, m_cbvSrvUavDescriptorSize);
    }
    m_GBuffer->CreateDescriptors();

    m_skyCubeSrvHeapStartIndex = m_GBufferTexturesSrvHeapStartIndex + GBuffer::EGBufferLayer::MAX;
    localHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(srvCpuStart, m_skyCubeSrvHeapStartIndex, m_cbvSrvUavDescriptorSize);
    {
        auto &texD3DResource = skyBoxResource;
        srvDesc.Format = texD3DResource->GetDesc().Format;
        srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
        srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
        srvDesc.TextureCube.MostDetailedMip = 0u;
        srvDesc.TextureCube.MipLevels = texD3DResource->GetDesc().MipLevels;
        srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
        m_device.CreateShaderResourceView(texD3DResource.Get(), &srvDesc, localHandle);

        localHandle.Offset(1, m_cbvSrvUavDescriptorSize);
    }
    m_texturesSrvHeapStartIndex = m_skyCubeSrvHeapStartIndex + 1;
}

void Blainn::RenderSubsystem::CreateRootSignature()
{
    m_rootSignature = eastl::make_shared<RootSignature>();

    CD3DX12_DESCRIPTOR_RANGE cascadeShadowSrv = {};
    cascadeShadowSrv.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1u, SHADER_REGISTER(0u), REGISTER_SPACE_1);

    CD3DX12_DESCRIPTOR_RANGE GBufferTable = {};
    GBufferTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, (UINT)GBuffer::EGBufferLayer::MAX, SHADER_REGISTER(1u), REGISTER_SPACE_1);

    CD3DX12_DESCRIPTOR_RANGE skyBoxTable = {};
    skyBoxTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1u /*(UINT)m_skyTextures.size()*/, SHADER_REGISTER(6u), REGISTER_SPACE_1);

    // Bindless unbound textures
    CD3DX12_DESCRIPTOR_RANGE textureTable = {};
    textureTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, (UINT)-1, SHADER_REGISTER(7u), REGISTER_SPACE_1);

    // Root parameter can be a table, root descriptor or root constants.
    CD3DX12_ROOT_PARAMETER slotRootParameter[RootSignature::ERootParam::NumRootParameters];

    // Perfomance TIP: Order from most frequent to least frequent.
    slotRootParameter[RootSignature::ERootParam::PerObjectDataCB].InitAsConstantBufferView(SHADER_REGISTER(0u), REGISTER_SPACE_0, D3D12_SHADER_VISIBILITY_ALL);
    slotRootParameter[RootSignature::ERootParam::PerPassDataCB].InitAsConstantBufferView(SHADER_REGISTER(1u), REGISTER_SPACE_0, D3D12_SHADER_VISIBILITY_ALL);

    slotRootParameter[RootSignature::ERootParam::MaterialsDataSB].InitAsShaderResourceView(SHADER_REGISTER(0u), REGISTER_SPACE_0, D3D12_SHADER_VISIBILITY_ALL);
    slotRootParameter[RootSignature::ERootParam::PointLightsDataSB].InitAsShaderResourceView(SHADER_REGISTER(1u), REGISTER_SPACE_0, D3D12_SHADER_VISIBILITY_ALL);
    slotRootParameter[RootSignature::ERootParam::SpotLightsDataSB].InitAsShaderResourceView(SHADER_REGISTER(2u), REGISTER_SPACE_0, D3D12_SHADER_VISIBILITY_ALL);

    slotRootParameter[RootSignature::ERootParam::CascadedShadowMaps].InitAsDescriptorTable(1u, &cascadeShadowSrv, D3D12_SHADER_VISIBILITY_PIXEL);
    slotRootParameter[RootSignature::ERootParam::GBufferTextures].InitAsDescriptorTable(1u, &GBufferTable, D3D12_SHADER_VISIBILITY_PIXEL);
    slotRootParameter[RootSignature::ERootParam::SkyBox].InitAsDescriptorTable(1u, &skyBoxTable, D3D12_SHADER_VISIBILITY_PIXEL);
    slotRootParameter[RootSignature::ERootParam::Textures].InitAsDescriptorTable(1u, &textureTable, D3D12_SHADER_VISIBILITY_PIXEL);

    m_rootSignature->Create(m_device, ARRAYSIZE(slotRootParameter), slotRootParameter, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

#pragma region UUID
    m_UUIDRootSignature = eastl::make_shared<RootSignature>();
    CD3DX12_ROOT_PARAMETER uuidRootParameter[2];
    // mat4 + 128(32 * 4) bit uuid
    uuidRootParameter[0].InitAsConstants(20, SHADER_REGISTER(0));
    // mat4 viewproj
    uuidRootParameter[1].InitAsConstants(16, SHADER_REGISTER(1));
    // uuidRootParameter[0].InitAsConstantBufferView(0, 0);
    // uuidRootParameter[1].InitAsConstantBufferView(1, 0);
    m_UUIDRootSignature->Create(m_device, ARRAYSIZE(uuidRootParameter), uuidRootParameter, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
#pragma endregion
}

void Blainn::RenderSubsystem::CreateShaders()
{
    const D3D_SHADER_MACRO fogDefines[] = {"FOG", "1", NULL, NULL};

    const D3D_SHADER_MACRO alphaTestDefines[] = {"ALPHA_TEST", "1", "FOG", "1", NULL, NULL};

    const D3D_SHADER_MACRO shadowDebugDefines[] = {"SHADOW_DEBUG", "1", NULL, NULL};

    m_shaders[Shader::EShaderType::CascadedShadowsVS] = FreyaUtil::CompileShader(L"./Content/Shaders/ShadowVS.hlsl", nullptr, "main", "vs_5_1");
    m_shaders[Shader::EShaderType::CascadedShadowsGS] = FreyaUtil::CompileShader(L"./Content/Shaders/CascadesGS.hlsl", nullptr, "main", "gs_5_1");

#pragma region DeferredShading
    m_shaders[Shader::EShaderType::DeferredGeometryVS] = FreyaUtil::CompileShader(L"./Content/Shaders/GBufferPassVS.hlsl", nullptr, "main", "vs_5_1");
    m_shaders[Shader::EShaderType::DeferredGeometryPS] = FreyaUtil::CompileShader(L"./Content/Shaders/GBufferPassPS.hlsl", nullptr, "main", "ps_5_1");

    m_shaders[Shader::EShaderType::DeferredDirVS] = FreyaUtil::CompileShader(L"./Content/Shaders/DeferredDirectionalLightVS.hlsl", nullptr, "main", "vs_5_1");
    m_shaders[Shader::EShaderType::DeferredDirPS] = FreyaUtil::CompileShader(L"./Content/Shaders/DeferredDirectionalLightPS.hlsl", nullptr, "main", "ps_5_1");

    m_shaders[Shader::EShaderType::DeferredLightVolumesVS] = FreyaUtil::CompileShader(L"./Content/Shaders/LightVolumesVS.hlsl", nullptr, "main", "vs_5_1");
    m_shaders[Shader::EShaderType::DeferredPointPS] = FreyaUtil::CompileShader(L"./Content/Shaders/DeferredPointLightPS.hlsl", nullptr, "main", "ps_5_1");
    m_shaders[Shader::EShaderType::DeferredSpotPS] = FreyaUtil::CompileShader(L"./Content/Shaders/DeferredSpotLightPS.hlsl", nullptr, "main", "ps_5_1");
#pragma endregion DeferredShading

#pragma region ForwardShading
#pragma region SkyBox
    m_shaders[Shader::EShaderType::SkyBoxVS] = FreyaUtil::CompileShader(L"./Content/Shaders/SkyBox.hlsl", nullptr, "VSMain", "vs_5_1");
    m_shaders[Shader::EShaderType::SkyBoxPS] = FreyaUtil::CompileShader(L"./Content/Shaders/SkyBox.hlsl", nullptr, "PSMain", "ps_5_1");
#pragma endregion SkyBox
#pragma endregion ForwardShading

#pragma region UUIDBuffer
    m_shaders[Shader::EShaderType::UUIDVS] = FreyaUtil::CompileShader(L"./Content/Shaders/UUID.hlsl", nullptr, "VSMain", "vs_5_1");
    m_shaders[Shader::EShaderType::UUIDPS] = FreyaUtil::CompileShader(L"./Content/Shaders/UUID.hlsl", nullptr, "PSMain", "ps_5_1");
#pragma endregion UUIDBuffer
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
    defaultPsoDesc.SampleDesc = {1u, 0u}; // No MSAA. This should match the setting of the render target we are using (check swapChainDesc)

#pragma region CascadeShadowsDepthPass
    D3D12_GRAPHICS_PIPELINE_STATE_DESC cascadeShadowPsoDesc = defaultPsoDesc;

    cascadeShadowPsoDesc.VS = D3D12_SHADER_BYTECODE(
        {reinterpret_cast<BYTE *>(m_shaders.at(Shader::EShaderType::CascadedShadowsVS)->GetBufferPointer()),
         m_shaders.at(Shader::EShaderType::CascadedShadowsVS)->GetBufferSize()});
    cascadeShadowPsoDesc.GS = D3D12_SHADER_BYTECODE(
        {reinterpret_cast<BYTE *>(m_shaders.at(Shader::EShaderType::CascadedShadowsGS)->GetBufferPointer()),
         m_shaders.at(Shader::EShaderType::CascadedShadowsGS)->GetBufferSize()});

    cascadeShadowPsoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
    cascadeShadowPsoDesc.RasterizerState.DepthBias = 10000;
    cascadeShadowPsoDesc.RasterizerState.DepthClipEnable = (BOOL)0.0f;
    cascadeShadowPsoDesc.RasterizerState.SlopeScaledDepthBias = 1.0f;
    cascadeShadowPsoDesc.InputLayout = SimpleVertex::InputLayout;
    cascadeShadowPsoDesc.NumRenderTargets = 0u;
    cascadeShadowPsoDesc.RTVFormats[0] = DXGI_FORMAT_UNKNOWN;
    ThrowIfFailed(m_device.CreateGraphicsPipelineState(
        cascadeShadowPsoDesc, m_pipelineStates[PipelineStateObject::EPsoType::CascadedShadowsOpaque]));
#pragma endregion CascadeShadowsDepthPass

#pragma region DeferredShading
    D3D12_GRAPHICS_PIPELINE_STATE_DESC GBufferPsoDesc = defaultPsoDesc;

    GBufferPsoDesc.VS = D3D12_SHADER_BYTECODE(
        {reinterpret_cast<BYTE *>(m_shaders.at(Shader::EShaderType::DeferredGeometryVS)->GetBufferPointer()),
         m_shaders.at(Shader::EShaderType::DeferredGeometryVS)->GetBufferSize()});
    GBufferPsoDesc.PS = D3D12_SHADER_BYTECODE(
        {reinterpret_cast<BYTE *>(m_shaders.at(Shader::EShaderType::DeferredGeometryPS)->GetBufferPointer()),
         m_shaders.at(Shader::EShaderType::DeferredGeometryPS)->GetBufferSize()});

    GBufferPsoDesc.NumRenderTargets = static_cast<UINT>(GBuffer::EGBufferLayer::MAX) - 1u;
    GBufferPsoDesc.RTVFormats[0] = m_GBuffer->GetBufferTextureFormat(GBuffer::EGBufferLayer::DIFFUSE_ALBEDO);
    GBufferPsoDesc.RTVFormats[1] = m_GBuffer->GetBufferTextureFormat(GBuffer::EGBufferLayer::AMBIENT_OCCLUSION);
    GBufferPsoDesc.RTVFormats[2] = m_GBuffer->GetBufferTextureFormat(GBuffer::EGBufferLayer::NORMAL);
    GBufferPsoDesc.RTVFormats[3] = m_GBuffer->GetBufferTextureFormat(GBuffer::EGBufferLayer::SPECULAR);
    ThrowIfFailed(m_device.CreateGraphicsPipelineState(
        GBufferPsoDesc, m_pipelineStates[PipelineStateObject::EPsoType::DeferredGeometry]));

    // not sure it works
#pragma region Wireframe
    D3D12_GRAPHICS_PIPELINE_STATE_DESC opaqueWireframe = GBufferPsoDesc;
    opaqueWireframe.RasterizerState.FillMode = D3D12_FILL_MODE_WIREFRAME;
    ThrowIfFailed(m_device.CreateGraphicsPipelineState(opaqueWireframe,
                                                       m_pipelineStates[PipelineStateObject::EPsoType::Wireframe]));
#pragma endregion Wireframe

#pragma region DeferredDirectional
    D3D12_GRAPHICS_PIPELINE_STATE_DESC dirLightPsoDesc = defaultPsoDesc;
    dirLightPsoDesc.VS = D3D12_SHADER_BYTECODE(
        {reinterpret_cast<BYTE *>(m_shaders.at(Shader::EShaderType::DeferredDirVS)->GetBufferPointer()),
         m_shaders.at(Shader::EShaderType::DeferredDirVS)->GetBufferSize()});
    dirLightPsoDesc.PS = D3D12_SHADER_BYTECODE(
        {reinterpret_cast<BYTE *>(m_shaders.at(Shader::EShaderType::DeferredDirPS)->GetBufferPointer()),
         m_shaders.at(Shader::EShaderType::DeferredDirPS)->GetBufferSize()});
    dirLightPsoDesc.InputLayout = SimpleVertex::InputLayout;
    ThrowIfFailed(m_device.CreateGraphicsPipelineState(
        dirLightPsoDesc, m_pipelineStates[PipelineStateObject::EPsoType::DeferredDirectional]));
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
        {reinterpret_cast<BYTE *>(m_shaders.at(Shader::EShaderType::DeferredLightVolumesVS)->GetBufferPointer()),
         m_shaders.at(Shader::EShaderType::DeferredLightVolumesVS)->GetBufferSize()});
    pointLightIntersectsFarPlanePsoDesc.PS = D3D12_SHADER_BYTECODE(
        {reinterpret_cast<BYTE *>(m_shaders.at(Shader::EShaderType::DeferredPointPS)->GetBufferPointer()),
         m_shaders.at(Shader::EShaderType::DeferredPointPS)->GetBufferSize()});
    pointLightIntersectsFarPlanePsoDesc.BlendState.AlphaToCoverageEnable = FALSE;
    pointLightIntersectsFarPlanePsoDesc.BlendState.IndependentBlendEnable = FALSE;
    pointLightIntersectsFarPlanePsoDesc.BlendState.RenderTarget[0] = RTBlendDesc;

    pointLightIntersectsFarPlanePsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.DepthEnable = FALSE;
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.StencilEnable = FALSE;
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.StencilReadMask = 0xFF;
    pointLightIntersectsFarPlanePsoDesc.DepthStencilState.StencilWriteMask = 0xFF;
    ThrowIfFailed(m_device.CreateGraphicsPipelineState(pointLightIntersectsFarPlanePsoDesc, m_pipelineStates[PipelineStateObject::EPsoType::DeferredPointIntersectsFarPlane]));

    D3D12_GRAPHICS_PIPELINE_STATE_DESC pointLightWithinFrustumPsoDesc = pointLightIntersectsFarPlanePsoDesc;
    pointLightWithinFrustumPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
    pointLightWithinFrustumPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_GREATER;
    ThrowIfFailed(m_device.CreateGraphicsPipelineState(pointLightWithinFrustumPsoDesc, m_pipelineStates[PipelineStateObject::EPsoType::DeferredPointWithinFrustum]));

    //pointLightFullQuadPsoDesc

#pragma endregion DeferredPointLight

#pragma region DeferredSpotLight
    D3D12_GRAPHICS_PIPELINE_STATE_DESC spotLightIntersectsFarPlanePsoDesc = pointLightIntersectsFarPlanePsoDesc;
    spotLightIntersectsFarPlanePsoDesc.PS = D3D12_SHADER_BYTECODE(
        {reinterpret_cast<BYTE *>(m_shaders.at(Shader::EShaderType::DeferredSpotPS)->GetBufferPointer()),
         m_shaders.at(Shader::EShaderType::DeferredSpotPS)->GetBufferSize()});
    ThrowIfFailed(m_device.CreateGraphicsPipelineState(spotLightIntersectsFarPlanePsoDesc, m_pipelineStates[PipelineStateObject::EPsoType::DeferredSpotIntersectsFarPlane]));

    //spotLightWithinFrustumPsoDesc
    //spotLightFullQuadPsoDesc

#pragma endregion DeferredSpotLight

#pragma endregion DeferredShading

#pragma region Sky
    D3D12_GRAPHICS_PIPELINE_STATE_DESC skyPsoDesc = dirLightPsoDesc;

    // The camera is inside the sky sphere, so just turn off culling.
    skyPsoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;

    // Make sure the depth function is LESS_EQUAL and not just LESS.
    // Otherwise, the normalized depth values at z = 1 (NDC) will
    // fail the depth test if the depth buffer was cleared to 1.
    skyPsoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    skyPsoDesc.DepthStencilState.DepthWriteMask =
        D3D12_DEPTH_WRITE_MASK_ZERO; // Disable writing explicitly (Depth still enable)
    skyPsoDesc.InputLayout = VertexPosition::InputLayout;
    skyPsoDesc.VS = {reinterpret_cast<BYTE *>(m_shaders.at(Shader::EShaderType::SkyBoxVS)->GetBufferPointer()),
                     m_shaders.at(Shader::EShaderType::SkyBoxVS)->GetBufferSize()};
    skyPsoDesc.PS = {reinterpret_cast<BYTE *>(m_shaders.at(Shader::EShaderType::SkyBoxPS)->GetBufferPointer()),
                     m_shaders.at(Shader::EShaderType::SkyBoxPS)->GetBufferSize()};
    ThrowIfFailed(
        m_device.CreateGraphicsPipelineState(skyPsoDesc, m_pipelineStates[PipelineStateObject::EPsoType::Sky]));
#pragma endregion Sky

#pragma region DebugDraw
    D3D12_GRAPHICS_PIPELINE_STATE_DESC debugPsoDesc = defaultPsoDesc;
    debugPsoDesc.InputLayout = DebugVertex::InputLayout;
    // set debug shaders
    // debugPsoDesc.VS =
    // debugPsoDesc.PS =
    // ThrowIfFailed(m_device.CreateGraphicsPipelineState(debugPsoDesc,
    // m_pipelineStates[PipelineStateObject::EPsoType::Debug]));
#pragma endregion DebugDraw

#pragma region Outline
    D3D12_DEPTH_STENCILOP_DESC stencilOpReplace = {};
    stencilOpReplace.StencilFailOp = D3D12_STENCIL_OP_REPLACE;
    stencilOpReplace.StencilDepthFailOp = D3D12_STENCIL_OP_REPLACE;
    stencilOpReplace.StencilPassOp = D3D12_STENCIL_OP_REPLACE;
    stencilOpReplace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

    D3D12_DEPTH_STENCILOP_DESC stencilOpKeep = {};
    stencilOpKeep.StencilFailOp = D3D12_STENCIL_OP_KEEP;
    stencilOpKeep.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
    stencilOpKeep.StencilPassOp = D3D12_STENCIL_OP_KEEP;
    stencilOpKeep.StencilFunc = D3D12_COMPARISON_FUNC_NOT_EQUAL;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC outlineWritePsoDesc = defaultPsoDesc;
    outlineWritePsoDesc.DepthStencilState.DepthEnable = FALSE;
    outlineWritePsoDesc.DepthStencilState.StencilEnable = TRUE;
    outlineWritePsoDesc.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_WRITE_MASK;
    outlineWritePsoDesc.DepthStencilState.BackFace = stencilOpReplace;
    outlineWritePsoDesc.DepthStencilState.FrontFace = stencilOpReplace;

    D3D12_GRAPHICS_PIPELINE_STATE_DESC outlineReadPsoDesc = outlineWritePsoDesc;
    outlineReadPsoDesc.DepthStencilState.DepthEnable = FALSE;
    outlineReadPsoDesc.DepthStencilState.StencilEnable = TRUE;
    outlineReadPsoDesc.DepthStencilState.StencilWriteMask = D3D12_DEFAULT_STENCIL_READ_MASK;
    outlineReadPsoDesc.DepthStencilState.FrontFace = stencilOpKeep;
    outlineReadPsoDesc.DepthStencilState.BackFace = stencilOpKeep;

#pragma endregion

#pragma region UUID

    D3D12_GRAPHICS_PIPELINE_STATE_DESC uuidDrawPSO;
    ZeroMemory(&uuidDrawPSO, sizeof(uuidDrawPSO));
    uuidDrawPSO.pRootSignature = m_UUIDRootSignature->Get();

    uuidDrawPSO.VS = {static_cast<BYTE *>(m_shaders.at(Shader::EShaderType::UUIDVS)->GetBufferPointer()),
                      m_shaders.at(Shader::EShaderType::UUIDVS)->GetBufferSize()};
    uuidDrawPSO.PS = {static_cast<BYTE *>(m_shaders.at(Shader::EShaderType::UUIDPS)->GetBufferPointer()),
                      m_shaders.at(Shader::EShaderType::UUIDPS)->GetBufferSize()};

    uuidDrawPSO.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // Blend state is disable
    uuidDrawPSO.SampleMask = UINT_MAX;
    uuidDrawPSO.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);

    D3D12_DEPTH_STENCIL_DESC uuidDepthDesc;
    uuidDepthDesc.DepthEnable = TRUE;
    uuidDepthDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
    uuidDepthDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
    uuidDepthDesc.StencilEnable = FALSE;
    uuidDrawPSO.DepthStencilState = uuidDepthDesc;

    uuidDrawPSO.InputLayout = SimpleVertex::InputLayout;
    uuidDrawPSO.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
    uuidDrawPSO.NumRenderTargets = 1u;
    uuidDrawPSO.RTVFormats[0] = DXGI_FORMAT_R32G32B32A32_UINT;
    uuidDrawPSO.DSVFormat = DepthStencilFormat;
    uuidDrawPSO.SampleDesc = {1u, 0u};

    ThrowIfFailed(m_device.CreateGraphicsPipelineState(uuidDrawPSO, m_pipelineStates[PipelineStateObject::EPsoType::UUID]));

#pragma endregion
}

#pragma region Update
void Blainn::RenderSubsystem::UpdateObjectsCB(float deltaTime)
{
    BLAINN_PROFILE_FUNC();
    const auto &renderEntitiesView = Engine::GetActiveScene()->GetAllEntitiesWith<IDComponent, TransformComponent, MeshComponent>();
    for (const auto &[entity, entityID, entityTransform, entityMesh] : renderEntitiesView.each())
    {
        const auto &_entity = Engine::GetActiveScene()->TryGetEntityWithUUID(entityID.ID);
        if (!_entity.IsValid()) continue;

        if (entityTransform.IsFramesDirty() || entityMesh.MaterialHandle->GetMaterial().IsFramesDirty())
        {
            ObjectConstants objConstants;

            auto world = Engine::GetActiveScene()->GetWorldSpaceTransformMatrix(_entity);
            auto transposeWorld = world.Transpose();
            auto invTransposeWorld = transposeWorld.Invert();

            XMStoreFloat4x4(&objConstants.World, transposeWorld);
            XMStoreFloat4x4(&objConstants.InvTransposeWorld, XMMatrixTranspose(invTransposeWorld));
            XMStoreFloat4x4(&objConstants.TexTransform, XMMatrixTranspose(entityMesh.MeshHandle->GetMesh().GetTextureTransform()));
            objConstants.MaterialIndex = entityMesh.MaterialHandle->GetIndex();

            entityMesh.UpdateMeshCB(objConstants);
            entityTransform.FrameResetDirtyFlags();
        }
    }
}

void RenderSubsystem::UpdateLightsBuffers(float deltaTime)
{
    BLAINN_PROFILE_FUNC();
#pragma region PointLights
    auto currPointLightSB = m_currFrameResource->PointLightSB.get();

    const auto &pointLightEntitiesView = Engine::GetActiveScene()->GetAllEntitiesWith<IDComponent, TransformComponent, PointLightComponent>();
    for (const auto &[entity, entityID, entityTransform, entityLight] : pointLightEntitiesView.each())
    {
        const auto &_entity = Engine::GetActiveScene()->TryGetEntityWithUUID(entityID.ID);
        if (!_entity.IsValid()) continue;

        //if (!entityTransform.IsFramesDirty() && !entityLight.IsFramesDirty()) continue;

        PointLightInstanceData m_perInstanceSBData;

        auto world = Engine::GetActiveScene()->GetWorldSpaceTransformMatrix(_entity);
        XMStoreFloat4x4(&m_perInstanceSBData.World, XMMatrixTranspose(world));
        m_perInstanceSBData.Light.Color = entityLight.Color;
        m_perInstanceSBData.Light.FalloffEnd = entityLight.FalloffEnd; // range
        m_perInstanceSBData.Light.FalloffStart = entityLight.FalloffStart;
        m_perInstanceSBData.Light.Position = entityTransform.GetTranslation();

        currPointLightSB->CopyData(m_pointLightsCount++, m_perInstanceSBData);

        entityTransform.FrameResetDirtyFlags();
        entityLight.FrameResetDirtyFlags();
    }
#pragma endregion PointLights

#pragma region SpotLights
    //const auto &spotLightEntitiesView = Engine::GetActiveScene()->GetAllEntitiesWith<TransformComponent, SpotLightComponent>();
    //for (const auto &[entity, transform, entityLight] : spotLightEntitiesView.each())
    //{
        //++m_spotLightsCount;
        //if (!entityTransform.IsFramesDirty() /*|| !entityLight.IsFramesDirty()*/) continue;

        //SpotLightInstanceData m_perInstanceSBData;
    //}
#pragma endregion SpotLights
}

void Blainn::RenderSubsystem::UpdateMaterialBuffer(float deltaTime)
{
    BLAINN_PROFILE_FUNC();
    auto currMaterialDataSB = m_currFrameResource->MaterialSB.get();

    auto &materials = AssetManager::GetInstance().m_materials;

    for (int matIndex = 0; matIndex < materials.size(); ++matIndex)
    {
        m_perMaterialSBData = MaterialData();
        if (materials[matIndex] /* && materials[matIndex]->IsFramesDirty()*/)
        {
            XMStoreFloat4x4(&m_perMaterialSBData.MatTransform, XMMatrixTranspose(materials[matIndex]->GetMaterialTransform()));

            m_perMaterialSBData.DiffuseAlbedo = materials[matIndex]->GetDefaultAldedo();
            // m_perMaterialSBData.FresnelR0 = mat->FresnelR0;
            m_perMaterialSBData.Roughness = materials[matIndex]->GetDefaultRougnessScale();
            m_perMaterialSBData.DiffuseMapIndex =
                materials[matIndex]->HasTexture(TextureType::ALBEDO)
                    ? materials[matIndex]->GetTextureHandle(TextureType::ALBEDO).GetIndex()
                    : static_cast<uint32_t>(-1);
            m_perMaterialSBData.NormalMapIndex =
                materials[matIndex]->HasTexture(TextureType::NORMAL)
                    ? materials[matIndex]->GetTextureHandle(TextureType::NORMAL).GetIndex()
                    : static_cast<uint32_t>(-1);
            m_perMaterialSBData.RoughnessMapIndex =
                materials[matIndex]->HasTexture(TextureType::ROUGHNESS)
                    ? materials[matIndex]->GetTextureHandle(TextureType::ROUGHNESS).GetIndex()
                    : static_cast<uint32_t>(-1);
            m_perMaterialSBData.MetallicMapIndex =
                materials[matIndex]->HasTexture(TextureType::METALLIC)
                    ? materials[matIndex]->GetTextureHandle(TextureType::METALLIC).GetIndex()
                    : static_cast<uint32_t>(-1);

            // m_perMaterialSBData.AOMapIndex = materials[matIndex]->GetTextureHandle(TextureType::AO).GetIndex();

            //currMaterialDataSB->CopyData(matIndex, m_perMaterialSBData);

            materials[matIndex]->FrameResetDirtyFlags();
        }
        currMaterialDataSB->CopyData(matIndex, m_perMaterialSBData);
    }
}

void Blainn::RenderSubsystem::UpdateShadowTransform(float deltaTime)
{
    BLAINN_PROFILE_FUNC();
    static eastl::array<eastl::pair<XMMATRIX, XMMATRIX>, MaxCascades> lightSpaceMatrices;
    GetLightSpaceMatrices(lightSpaceMatrices);

    for (UINT i = 0; i < MaxCascades; ++i)
    {
        XMMATRIX shadowTransform = lightSpaceMatrices[i].first * lightSpaceMatrices[i].second;
        m_shadowPassCBData.Cascades.CascadeViewProj[i] = XMMatrixTranspose(shadowTransform);

        m_deferredPassCBData.Cascades.CascadeViewProj[i] = XMMatrixTranspose(shadowTransform);
        m_deferredPassCBData.Cascades.Distances[i] = m_cascadeShadowMap->GetCascadeLevel(i);
    }
}

void Blainn::RenderSubsystem::UpdateShadowPassCB(float deltaTime)
{
    BLAINN_PROFILE_FUNC();
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
    BLAINN_PROFILE_FUNC();
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

void Blainn::RenderSubsystem::UpdateDeferredPassCB(float deltaTime)
{
    BLAINN_PROFILE_FUNC();
    XMMATRIX view = m_camera->GetViewMatrix();
    XMMATRIX proj = m_camera->GetPerspectiveProjectionMatrix();
    XMMATRIX viewProj = XMMatrixMultiply(view, proj);
    auto det = XMMatrixDeterminant(viewProj);
    XMMATRIX invViewProj = XMMatrixInverse(&det, viewProj);

    XMStoreFloat4x4(&m_deferredPassCBData.View, XMMatrixTranspose(view));
    XMStoreFloat4x4(&m_deferredPassCBData.Proj, XMMatrixTranspose(proj));
    XMStoreFloat4x4(&m_deferredPassCBData.ViewProj, XMMatrixTranspose(viewProj));
    XMStoreFloat4x4(&m_deferredPassCBData.InvViewProj, XMMatrixTranspose(invViewProj));

    m_deferredPassCBData.EyePosW = m_camera->GetPosition3f();
    m_deferredPassCBData.RenderTargetSize = XMFLOAT2((float)m_width, (float)m_height);
    m_deferredPassCBData.InvRenderTargetSize = XMFLOAT2(1.0f / m_width, 1.0f / m_height);
    m_deferredPassCBData.NearZ = m_camera->GetNearZ();
    m_deferredPassCBData.FarZ = m_camera->GetFarZ();
    m_deferredPassCBData.DeltaTime = deltaTime;
    m_deferredPassCBData.TotalTime = deltaTime;

    m_deferredPassCBData.Ambient = {0.25f, 0.25f, 0.35f, 1.0f};

#pragma region DirLight
    // Invert sign because other way light would be pointing up
    const auto &dirLightEntitiesView = Engine::GetActiveScene()->GetAllEntitiesWith<TransformComponent, DirectionalLightComponent>();
    for (const auto &[entity, entityTransform, entityLight] : dirLightEntitiesView.each())
    {
        m_deferredPassCBData.DirLight.Color = entityLight.Color;
        m_deferredPassCBData.DirLight.Direction = entityTransform.GetForwardVector();
    }
#pragma endregion DirLight

    auto currPassCB = m_currFrameResource->PassCB.get();
    currPassCB->CopyData(static_cast<int>(EPassType::DeferredLighting), m_deferredPassCBData);
}

void RenderSubsystem::UpdateForwardPassCB(float deltaTime)
{
    XMMATRIX view = m_camera->GetViewMatrix();
    XMMATRIX proj = m_camera->GetPerspectiveProjectionMatrix();
    XMMATRIX viewProj = XMMatrixMultiply(view, proj);
    auto det = XMMatrixDeterminant(viewProj);
    XMMATRIX invViewProj = XMMatrixInverse(&det, viewProj);

    XMStoreFloat4x4(&m_deferredPassCBData.View, XMMatrixTranspose(view));
    XMStoreFloat4x4(&m_deferredPassCBData.Proj, XMMatrixTranspose(proj));
    XMStoreFloat4x4(&m_deferredPassCBData.ViewProj, XMMatrixTranspose(viewProj));
    XMStoreFloat4x4(&m_deferredPassCBData.InvViewProj, XMMatrixTranspose(invViewProj));

    m_deferredPassCBData.EyePosW = m_camera->GetPosition3f();
    m_deferredPassCBData.RenderTargetSize = XMFLOAT2((float)m_width, (float)m_height);
    m_deferredPassCBData.InvRenderTargetSize = XMFLOAT2(1.0f / m_width, 1.0f / m_height);
    m_deferredPassCBData.NearZ = m_camera->GetNearZ();
    m_deferredPassCBData.FarZ = m_camera->GetFarZ();
    m_deferredPassCBData.DeltaTime = deltaTime;
    m_deferredPassCBData.TotalTime = deltaTime;

    m_deferredPassCBData.Ambient = {0.25f, 0.25f, 0.35f, 1.0f};
}
#pragma endregion Update

void Blainn::RenderSubsystem::ResourceBarrier(ID3D12GraphicsCommandList2 *pCommandList, ID3D12Resource *pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter)
{
    auto transition = CD3DX12_RESOURCE_BARRIER::Transition(pResource, stateBefore, stateAfter);
    pCommandList->ResourceBarrier(1u, &transition);
}

void Blainn::RenderSubsystem::RenderDepthOnlyPass(ID3D12GraphicsCommandList2 *pCommandList)
{
    BLAINN_PROFILE_FUNC();
    UINT passCBByteSize = FreyaUtil::CalcConstantBufferByteSize(sizeof(PassConstants));

    auto csmViewport = m_cascadeShadowMap->GetViewport();
    auto csmScissor = m_cascadeShadowMap->GetScissorRect();

    pCommandList->RSSetViewports(1u, &csmViewport);
    pCommandList->RSSetScissorRects(1u, &csmScissor);

    // change to depth write state
    ResourceBarrier(pCommandList, m_cascadeShadowMap->Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);

#pragma region BypassResources
    auto currFramePassCB = m_currFrameResource->PassCB->Get();
    auto currFrameGPUVirtualAddress = FreyaUtil::GetGPUVirtualAddress(
        currFramePassCB->GetGPUVirtualAddress(), passCBByteSize, static_cast<UINT>(EPassType::DepthShadow));
    pCommandList->SetGraphicsRootConstantBufferView(RootSignature::ERootParam::PerPassDataCB, currFrameGPUVirtualAddress);
#pragma endregion BypassResources

    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_cascadeShadowMap->GetDsv());
    pCommandList->OMSetRenderTargets(0u, nullptr, TRUE, &dsvHandle);
    pCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0u, 0u, nullptr);

    pCommandList->SetPipelineState(m_pipelineStates.at(PipelineStateObject::EPsoType::CascadedShadowsOpaque).Get());
    DrawMeshes(pCommandList);

    ResourceBarrier(pCommandList, m_cascadeShadowMap->Get(), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void Blainn::RenderSubsystem::RenderGeometryPass(ID3D12GraphicsCommandList2 *pCommandList)
{
    BLAINN_PROFILE_FUNC();
    UINT passCBByteSize = FreyaUtil::CalcConstantBufferByteSize(sizeof(PassConstants));
    // The viewport needs to be reset whenever the command list is reset.
    pCommandList->RSSetViewports(1u, &m_viewport);
    pCommandList->RSSetScissorRects(1u, &m_scissorRect);

    for (unsigned i = 0; i < GBuffer::EGBufferLayer::MAX - 1u; i++)
    {
        ResourceBarrier(pCommandList, m_GBuffer->Get(i), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_RENDER_TARGET);
    }
    ResourceBarrier(pCommandList, m_GBuffer->Get(GBuffer::EGBufferLayer::DEPTH), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_WRITE);

#pragma region BypassResources
    auto currFramePassCB = m_currFrameResource->PassCB->Get();
    auto currFramePassCBAddress = FreyaUtil::GetGPUVirtualAddress(
        currFramePassCB->GetGPUVirtualAddress(), passCBByteSize, static_cast<UINT>(EPassType::DeferredGeometry));
    pCommandList->SetGraphicsRootConstantBufferView(RootSignature::ERootParam::PerPassDataCB, currFramePassCBAddress);

    // Bind all the materials used in this scene. For structured buffers, we can bypass the heap and set as a root
    // descriptor.
    auto matBuffer = m_currFrameResource->MaterialSB->Get();
    pCommandList->SetGraphicsRootShaderResourceView(RootSignature::ERootParam::MaterialsDataSB, matBuffer->GetGPUVirtualAddress());

    // Bind all the textures used in this scene. Observe that we only have to specify the first descriptor in the table.
    // The root signature knows how many descriptors are expected in the table.
    pCommandList->SetGraphicsRootDescriptorTable(
        RootSignature::ERootParam::Textures,
        CD3DX12_GPU_DESCRIPTOR_HANDLE(m_srvHeap->GetGPUDescriptorHandleForHeapStart(), m_texturesSrvHeapStartIndex, m_cbvSrvUavDescriptorSize));
#pragma endregion BypassResources

    // start of the GBuffer rtvs in rtvHeap
    CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), SwapChainFrameCount, m_rtvDescriptorSize);
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_GBuffer->GetDsv(GBuffer::EGBufferLayer::DEPTH));
    pCommandList->OMSetRenderTargets(GBuffer::EGBufferLayer::DEPTH, &rtvHandle, TRUE, &dsvHandle);

    const float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    pCommandList->ClearRenderTargetView(m_GBuffer->GetRtv(GBuffer::EGBufferLayer::DIFFUSE_ALBEDO), Colors::LightSteelBlue, 0u, nullptr);
    pCommandList->ClearRenderTargetView(m_GBuffer->GetRtv(GBuffer::EGBufferLayer::AMBIENT_OCCLUSION), clearColor, 0u, nullptr);
    pCommandList->ClearRenderTargetView(m_GBuffer->GetRtv(GBuffer::EGBufferLayer::NORMAL), clearColor, 0u, nullptr);
    pCommandList->ClearRenderTargetView(m_GBuffer->GetRtv(GBuffer::EGBufferLayer::SPECULAR), clearColor, 0u, nullptr);
    pCommandList->ClearDepthStencilView(m_GBuffer->GetDsv(GBuffer::EGBufferLayer::DEPTH), D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0u, 0u, nullptr);

    pCommandList->SetPipelineState(m_pipelineStates.at(PipelineStateObject::EPsoType::DeferredGeometry).Get());

    DrawMeshes(pCommandList);

    for (unsigned i = 0; i < GBuffer::EGBufferLayer::MAX - 1u; i++)
    {
        ResourceBarrier(pCommandList, m_GBuffer->Get(i), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_GENERIC_READ);
    }
    ResourceBarrier(pCommandList, m_GBuffer->Get(GBuffer::EGBufferLayer::DEPTH), D3D12_RESOURCE_STATE_DEPTH_WRITE, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void Blainn::RenderSubsystem::RenderLightingPass(ID3D12GraphicsCommandList2 *pCommandList)
{
    BLAINN_PROFILE_FUNC();
    DeferredDirectionalLightPass(pCommandList);
    DeferredPointLightPass(pCommandList);
    // DeferredSpotLightPass(pCommandList);
}

void Blainn::RenderSubsystem::DeferredDirectionalLightPass(ID3D12GraphicsCommandList2 *pCommandList)
{
    UINT passCBByteSize = FreyaUtil::CalcConstantBufferByteSize(sizeof(PassConstants));

    // Indicate that the back buffer will be used as a render target.
    ResourceBarrier(pCommandList, m_swapChain->GetBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);

    // The viewport needs to be reset whenever the command list is reset.
    pCommandList->RSSetViewports(1u, &m_viewport);
    pCommandList->RSSetScissorRects(1u, &m_scissorRect);

    auto rtvHandle = GetRTV();
    auto dsvHandle = GetDSV();

    const float clearColor[4] = {0.7f, 0.7f, 0.7f, 1.0f};
    pCommandList->OMSetRenderTargets(1u, &rtvHandle, TRUE, &dsvHandle);
    pCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0u, nullptr);
    pCommandList->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.0f, 0u, 0u, nullptr);

#pragma region BypassResources
    auto currFramePassCB = m_currFrameResource->PassCB->Get();
    auto currFramePassCBAddress = FreyaUtil::GetGPUVirtualAddress(
        currFramePassCB->GetGPUVirtualAddress(), passCBByteSize, static_cast<UINT>(EPassType::DeferredLighting));
    pCommandList->SetGraphicsRootConstantBufferView(RootSignature::ERootParam::PerPassDataCB, currFramePassCBAddress);

    auto srvGpuStart = m_srvHeap->GetGPUDescriptorHandleForHeapStart();
    // Set shaadow map texture for main pass
    pCommandList->SetGraphicsRootDescriptorTable(
        RootSignature::ERootParam::CascadedShadowMaps,
        CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, m_cascadesShadowSrvHeapStartIndex, m_cbvSrvUavDescriptorSize));
    // Bind GBuffer textures
    pCommandList->SetGraphicsRootDescriptorTable(
        RootSignature::ERootParam::GBufferTextures,
        CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, m_GBufferTexturesSrvHeapStartIndex, m_cbvSrvUavDescriptorSize));
    // Bind SkyBox for sky reflections
    pCommandList->SetGraphicsRootDescriptorTable(
        RootSignature::ERootParam::SkyBox,
        CD3DX12_GPU_DESCRIPTOR_HANDLE(srvGpuStart, m_skyCubeSrvHeapStartIndex, m_cbvSrvUavDescriptorSize));
#pragma endregion BypassResources

    pCommandList->SetPipelineState(m_pipelineStates.at(PipelineStateObject::EPsoType::DeferredDirectional).Get());
    DrawQuad(pCommandList);
}

void Blainn::RenderSubsystem::DeferredPointLightPass(ID3D12GraphicsCommandList2 *pCommandList)
{
    auto instanceBuffer = m_currFrameResource->PointLightSB->Get();
    pCommandList->SetGraphicsRootShaderResourceView(RootSignature::ERootParam::PointLightsDataSB, instanceBuffer->GetGPUVirtualAddress());

    // !!! HACK (TO DRAW EVEN IF FRUSTUM INTERSECTS LIGHT VOLUME)
    pCommandList->SetPipelineState(m_pipelineStates.at(PipelineStateObject::EPsoType::DeferredPointWithinFrustum).Get());

    DrawInstancedMesh(pCommandList, AssetManager::GetInstance().GetDefaultModel(static_cast<uint32_t>(EPrebuiltMeshType::SPHERE)), m_pointLightsCount);
    m_pointLightsCount = 0u;
}

void Blainn::RenderSubsystem::DeferredSpotLightPass(ID3D12GraphicsCommandList2 *pCommandList)
{
    auto instanceBuffer = m_currFrameResource->SpotLightSB->Get();
    pCommandList->SetGraphicsRootShaderResourceView(RootSignature::ERootParam::SpotLightsDataSB, instanceBuffer->GetGPUVirtualAddress());

    // !!! HACK (TO DRAW EVEN IF FRUSTUM INTERSECTS LIGHT VOLUME)
    pCommandList->SetPipelineState(m_pipelineStates.at(PipelineStateObject::EPsoType::DeferredSpotWithinFrustum).Get());

    DrawInstancedMesh(pCommandList, AssetManager::GetInstance().GetDefaultModel(static_cast<uint32_t>(EPrebuiltMeshType::CONE)), m_spotLightsCount);
    m_spotLightsCount = 0u;
}

void Blainn::RenderSubsystem::RenderForwardPasses(ID3D12GraphicsCommandList2 *pCommandList)
{
    BLAINN_PROFILE_FUNC();
    // forward-like
    RenderSkyBoxPass(pCommandList);
    // RenderTransparencyPass(pCommandList);

    // Close accumulation buffer, that was opened in the light pass and indicate that the back buffer will now be used to present.
    ResourceBarrier(pCommandList, m_swapChain->GetBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
}

void Blainn::RenderSubsystem::RenderSkyBoxPass(ID3D12GraphicsCommandList2 *pCommandList)
{
    UINT passCBByteSize = FreyaUtil::CalcConstantBufferByteSize(sizeof(PassConstants));

    // Set GBuffer depth for read to properly draw sky box
    ResourceBarrier(pCommandList, m_GBuffer->Get(GBuffer::EGBufferLayer::DEPTH), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_READ);

    auto rtvHandle = GetRTV();
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart(), 2, m_dsvDescriptorSize);
    pCommandList->OMSetRenderTargets(1u, &rtvHandle, TRUE, &dsvHandle);

    auto currFramePassCB = m_currFrameResource->PassCB->Get();
    auto currFrameGPUVirtualAddress = FreyaUtil::GetGPUVirtualAddress(currFramePassCB->GetGPUVirtualAddress(), passCBByteSize, static_cast<UINT>(EPassType::DeferredLighting));
    pCommandList->SetGraphicsRootConstantBufferView(RootSignature::ERootParam::PerPassDataCB, currFrameGPUVirtualAddress);

    // Bind SkyBox texture
    pCommandList->SetGraphicsRootDescriptorTable(RootSignature::ERootParam::SkyBox, CD3DX12_GPU_DESCRIPTOR_HANDLE(m_srvHeap->GetGPUDescriptorHandleForHeapStart(), m_skyCubeSrvHeapStartIndex, m_cbvSrvUavDescriptorSize));
    pCommandList->SetPipelineState(m_pipelineStates.at(PipelineStateObject::EPsoType::Sky).Get());

    ObjectConstants obj;
    XMStoreFloat4x4(&obj.World, XMMatrixTranspose(XMMatrixScaling(5000.0f, 5000.0f, 5000.0f)));
    skyBox->UpdateMeshCB(obj);

    UINT objCBByteSize = (UINT)FreyaUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));
    auto currFrameObjCB = skyBox->ObjectCB->Get();
    D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = FreyaUtil::GetGPUVirtualAddress(currFrameObjCB->GetGPUVirtualAddress(), objCBByteSize, 0u);
    pCommandList->SetGraphicsRootConstantBufferView(RootSignature::ERootParam::PerObjectDataCB, objCBAddress);

    DrawMesh(pCommandList, AssetManager::GetInstance().GetDefaultModel(static_cast<uint32_t>(EPrebuiltMeshType::BOX)));

    ResourceBarrier(pCommandList, m_GBuffer->Get(GBuffer::EGBufferLayer::DEPTH), D3D12_RESOURCE_STATE_DEPTH_READ, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void Blainn::RenderSubsystem::RenderTransparencyPass(ID3D12GraphicsCommandList2 *pCommandList)
{

}

void RenderSubsystem::RenderDebugPass(ID3D12GraphicsCommandList2 *pCommandList)
{
    ResourceBarrier(pCommandList, m_swapChain->GetBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    ResourceBarrier(pCommandList, m_GBuffer->Get(GBuffer::EGBufferLayer::DEPTH), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_READ);
    m_debugRenderer->BeginDebugRenderPass(pCommandList, GetRTV(), m_GBuffer->GetDsv(GBuffer::EGBufferLayer::DEPTH));
    m_debugRenderer->SetViewProjMatrix(m_deferredPassCBData.ViewProj);

    auto view = Engine::GetActiveScene()->GetAllEntitiesWith<IDComponent, PhysicsComponent>();
    for (const auto &[_, id, physicsComponent] : view.each())
    {
        Entity entity = Engine::GetActiveScene()->GetEntityWithUUID(id.ID);
        auto bodyGetter = PhysicsSubsystem::GetBodyGetter(entity);

        switch (physicsComponent.GetShapeType())
        {
        case ComponentShapeType::Box:
        {
            auto min = bodyGetter.GetBoxShapeHalfExtents().value();
            Mat4 transformMatrix = Mat4::CreateFromQuaternion(bodyGetter.GetRotation())
                                   * Mat4::CreateTranslation(bodyGetter.GetPosition());

            m_debugRenderer->DrawWireBox(transformMatrix, min, -min, {0, 1, 0, 1});
            break;
        }
        case ComponentShapeType::Sphere:
        {
            float sphereRadius = bodyGetter.GetSphereShapeRadius().value();
            m_debugRenderer->DrawWireSphere(bodyGetter.GetPosition(), sphereRadius, {0, 1, 0, 1});
            break;
        }
        case ComponentShapeType::Capsule:
        {
            auto capsuleHalfHeightAndRadius = bodyGetter.GetCapsuleShapeHalfHeightAndRadius().value();
            Mat4 transformMatrix = Mat4::CreateFromQuaternion(bodyGetter.GetRotation())
                                   * Mat4::CreateTranslation(bodyGetter.GetPosition());

            m_debugRenderer->DrawCapsule(transformMatrix, capsuleHalfHeightAndRadius.first,
                                         capsuleHalfHeightAndRadius.second, {0, 1, 0, 1});
            break;
        }
        case ComponentShapeType::Cylinder:
        {
            auto cylinderHalfHeightAndRadius = bodyGetter.GetCylinderShapeHalfHeightAndRadius().value();
            Mat4 transformMatrix = Mat4::CreateFromQuaternion(bodyGetter.GetRotation())
                                   * Mat4::CreateTranslation(bodyGetter.GetPosition());

            m_debugRenderer->DrawCylinder(transformMatrix, cylinderHalfHeightAndRadius.first,
                                          cylinderHalfHeightAndRadius.second, {0, 1, 0, 1});
            break;
        }
        default:
            BF_ERROR("Unknown shape type");
        }
    }

    m_debugRenderer->EndDebugRenderPass();
    ResourceBarrier(pCommandList, m_swapChain->GetBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    ResourceBarrier(pCommandList, m_GBuffer->Get(GBuffer::EGBufferLayer::DEPTH), D3D12_RESOURCE_STATE_DEPTH_READ, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void RenderSubsystem::RenderUUIDPass(ID3D12GraphicsCommandList2 *pCommandList)
{
    ResourceBarrier(pCommandList, m_uuidRenderTarget.GetTexture(AttachmentPoint::Color0)->GetD3D12Resource().Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_RENDER_TARGET);
    ResourceBarrier(pCommandList, m_GBuffer->Get(GBuffer::EGBufferLayer::DEPTH), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_READ);

    const float clearColor[4] = {0.0f, 0.0f, 0.0f, 0.0f};
    pCommandList->ClearRenderTargetView(m_uuidRenderTarget.GetTexture(AttachmentPoint::Color0)->GetRenderTargetView(), clearColor, 0u, nullptr);

    pCommandList->SetGraphicsRootSignature(m_UUIDRootSignature->Get());
    pCommandList->SetPipelineState(m_pipelineStates[PipelineStateObject::EPsoType::UUID].Get());

    auto rtvHandle = m_uuidRenderTarget.GetTexture(AttachmentPoint::Color0)->GetRenderTargetView();
    CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle(m_dsvHeap->GetCPUDescriptorHandleForHeapStart(), 2, m_dsvDescriptorSize);
    pCommandList->OMSetRenderTargets(1, &rtvHandle, TRUE, &dsvHandle);
    pCommandList->RSSetViewports(1u, &m_viewport);
    pCommandList->RSSetScissorRects(1u, &m_scissorRect);

    Mat4 ViewProjMat = m_deferredPassCBData.ViewProj;
    pCommandList->SetGraphicsRoot32BitConstants(1, 16, &ViewProjMat, 0);

    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    auto view = Engine::GetActiveScene()->GetAllEntitiesWith<IDComponent, TransformComponent, MeshComponent>();
    for (const auto &[entity, idComponent, transformComponent, meshComponent] : view.each())
    {
        struct Data
        {
            Mat4 world;
            char id[16];
        } objData;
        Entity ent = Engine::GetActiveScene()->GetEntityWithUUID(idComponent.ID);
        objData.world = Engine::GetActiveScene()->GetWorldSpaceTransformMatrix(ent).Transpose();//transformComponent.GetTransform().Transpose();
        idComponent.ID.bytes(objData.id);

        pCommandList->SetGraphicsRoot32BitConstants(0, 20, &objData, 0);

        auto &model = meshComponent.MeshHandle->GetMesh();
        auto currVBV = model.VertexBufferView();
        auto currIBV = model.IndexBufferView();

        pCommandList->IASetVertexBuffers(0, 1, &currVBV);
        pCommandList->IASetIndexBuffer(&currIBV);

        [[likely]]
        if (currIBV.SizeInBytes)
        {
            pCommandList->DrawIndexedInstanced(model.GetIndicesCount(), 1u, 0u, 0u, 0u);
        }
        else
        {
            pCommandList->DrawInstanced(model.GetVerticesCount(), 1u, 0u, 0u);
        }
    }

    ResourceBarrier(pCommandList, m_uuidRenderTarget.GetTexture(AttachmentPoint::Color0)->GetD3D12Resource().Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_COMMON);
    ResourceBarrier(pCommandList, m_GBuffer->Get(GBuffer::EGBufferLayer::DEPTH), D3D12_RESOURCE_STATE_DEPTH_READ, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void RenderSubsystem::RenderImGuiPass(ID3D12GraphicsCommandList2 *pCommandList)
{
    ResourceBarrier(pCommandList, m_swapChain->GetBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
    ResourceBarrier(pCommandList, m_GBuffer->Get(GBuffer::EGBufferLayer::DEPTH), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_DEPTH_READ);

    m_UIRenderer->RenderUI(pCommandList);

    ResourceBarrier(pCommandList, m_swapChain->GetBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
    ResourceBarrier(pCommandList, m_GBuffer->Get(GBuffer::EGBufferLayer::DEPTH), D3D12_RESOURCE_STATE_DEPTH_READ, D3D12_RESOURCE_STATE_GENERIC_READ);
}

void Blainn::RenderSubsystem::DrawMesh(ID3D12GraphicsCommandList2 *pCommandList, const Model& mesh)
{
    auto currVBV = mesh.VertexBufferView();
    auto currIBV = mesh.IndexBufferView();

    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pCommandList->IASetVertexBuffers(0u, 1u, &currVBV);
    pCommandList->IASetIndexBuffer(&currIBV);

    [[likely]]
    if (currIBV.SizeInBytes)
    {
        pCommandList->DrawIndexedInstanced(mesh.GetIndicesCount(), 1u, 0u, 0u, 0u);
    }
    else
    {
        pCommandList->DrawInstanced(mesh.GetVerticesCount(), 1u, 0u, 0u);
    }
}

void Blainn::RenderSubsystem::DrawMeshes(ID3D12GraphicsCommandList2 *pCommandList)
{
    BLAINN_PROFILE_FUNC();
    auto commandQueue = m_device.GetCommandQueue();

    UINT objCBByteSize = (UINT)FreyaUtil::CalcConstantBufferByteSize(sizeof(ObjectConstants));

    const auto &renderEntitiesView = Engine::GetActiveScene()->GetAllEntitiesWith<IDComponent, TransformComponent, MeshComponent>();
    for (const auto &[entity, entityID, entityTransform, entityMesh] : renderEntitiesView.each())
    {
        if (!entityMesh.Enabled) continue;

        auto currObjectCB = entityMesh.ObjectCB->Get();

        auto &model = entityMesh.MeshHandle->GetMesh();

        auto currVBV = model.VertexBufferView();
        auto currIBV = model.IndexBufferView();

        pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        pCommandList->IASetVertexBuffers(0u, 1u, &currVBV);
        pCommandList->IASetIndexBuffer(&currIBV);

        D3D12_GPU_VIRTUAL_ADDRESS objCBAddress = FreyaUtil::GetGPUVirtualAddress(currObjectCB->GetGPUVirtualAddress(), objCBByteSize, 0u);
        pCommandList->SetGraphicsRootConstantBufferView(RootSignature::ERootParam::PerObjectDataCB, objCBAddress);

        [[likely]]
        if (currIBV.SizeInBytes)
        {
            pCommandList->DrawIndexedInstanced(model.GetIndicesCount(), 1u, 0u, 0u, 0u);
        }
        else
        {
            pCommandList->DrawInstanced(model.GetVerticesCount(), 1u, 0u, 0u);
        }
    }
}

void RenderSubsystem::DrawInstancedMesh(ID3D12GraphicsCommandList2 *pCommandList, const Model &mesh, const UINT numInstances)
{
    auto currVBV = mesh.VertexBufferView();
    auto currIBV = mesh.IndexBufferView();

    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
    pCommandList->IASetVertexBuffers(0u, 1u, &currVBV);
    pCommandList->IASetIndexBuffer(&currIBV);

    [[likely]]
    if (currIBV.SizeInBytes)
    {
        pCommandList->DrawIndexedInstanced(mesh.GetIndicesCount(), numInstances, 0u, 0, 0u);
    }
    else
    {
        pCommandList->DrawInstanced(mesh.GetVerticesCount(), numInstances, 0u, 0u);
    }

}

void Blainn::RenderSubsystem::DrawQuad(ID3D12GraphicsCommandList2 *pCommandList)
{
    pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    pCommandList->DrawInstanced(4u, 1u, 0u, 0u);
}

eastl::pair<XMMATRIX, XMMATRIX> Blainn::RenderSubsystem::GetLightSpaceMatrix(const float nearZ, const float farZ)
{
    const auto directionalLight = m_deferredPassCBData.DirLight;

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

void Blainn::RenderSubsystem::GetLightSpaceMatrices(eastl::array<eastl::pair<XMMATRIX, XMMATRIX>, 4>& outMatrices)
{
    for (UINT i = 0; i < MaxCascades; ++i)
    {
        if (i == 0)
            outMatrices[i] = (GetLightSpaceMatrix(m_camera->GetNearZ(), m_cascadeShadowMap->GetCascadeLevel(i)));
        else
            outMatrices[i] = (GetLightSpaceMatrix(m_cascadeShadowMap->GetCascadeLevel(i - 1), m_cascadeShadowMap->GetCascadeLevel(i)));
    }
}

eastl::vector<XMVECTOR> Blainn::RenderSubsystem::GetFrustumCornersWorldSpace(const XMMATRIX &view, const XMMATRIX &projection)
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
                const XMVECTOR pt = XMVector4Transform(XMVectorSet(2.0f * x - 1.0f, 2.0f * y - 1.0f, (float)z, 1.0f), invViewProj);
                frustumCorners.push_back(pt / XMVectorGetW(pt));
            }
        }
    }
    return frustumCorners;
}
} // namespace Blainn