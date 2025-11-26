#pragma once

#include "Render/Device.h"

Blainn::Device::Device(bool useWarpDevice)
    : m_useWarpDevice(useWarpDevice)
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

Blainn::Device::~Device()
{
    Flush();
}

VOID Blainn::Device::CreateDebugLayer()
{
    ComPtr<ID3D12Debug> debugController;
    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
    {
        debugController->EnableDebugLayer();

        // Enable additional debug layers.
        m_dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
    }
}

VOID Blainn::Device::Flush()
{
    m_directCommandQueue->Flush();
    m_copyCommandQueue->Flush();
    m_computeCommandQueue->Flush();
}

eastl::shared_ptr<Blainn::CommandQueue> Blainn::Device::GetCommandQueue(D3D12_COMMAND_LIST_TYPE commandListType) const
{
    switch (commandListType)
    {
    case D3D12_COMMAND_LIST_TYPE_DIRECT:
        return m_directCommandQueue;
        break;
    case D3D12_COMMAND_LIST_TYPE_COMPUTE:
        return m_computeCommandQueue;
        break;
    case D3D12_COMMAND_LIST_TYPE_COPY:
        return m_copyCommandQueue;
        break;
    default:
        assert(false && "Invalid command queue type.");
        return nullptr;
    }
}

// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
_Use_decl_annotations_ VOID Blainn::Device::GetHardwareAdapter(IDXGIFactory1 *pFactory, IDXGIAdapter1 **ppAdapter, bool requestHighPerformanceAdapter)
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

eastl::shared_ptr<Blainn::SwapChain> Blainn::Device::CreateSwapChain(HWND window, DXGI_FORMAT backBufferFormat)
{
    eastl::shared_ptr<SwapChain> swapChain = eastl::make_shared<SwapChain>(*this, window, backBufferFormat);

    return swapChain;
}

VOID Blainn::Device::CreateCommandQueues()
{
    // If we have multiple command queues, we can write a resource only from one queue at the same time.
    // Before it can be accessed by another queue, it must transition to read or common state.
    // In a read state resource can be read from multiple command queues simultaneously, including across processes,
    // based on its read state.
    m_directCommandQueue = eastl::make_shared<CommandQueue>(m_device, D3D12_COMMAND_LIST_TYPE_DIRECT);
    m_copyCommandQueue = eastl::make_shared<CommandQueue>(m_device, D3D12_COMMAND_LIST_TYPE_COPY);
    m_computeCommandQueue = eastl::make_shared<CommandQueue>(m_device, D3D12_COMMAND_LIST_TYPE_COMPUTE);
}

HRESULT Blainn::Device::CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE commandListType, ComPtr<ID3D12CommandAllocator>& commandAllocator)
{
    return m_device->CreateCommandAllocator(commandListType, IID_PPV_ARGS(commandAllocator.GetAddressOf()));
}

HRESULT Blainn::Device::CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors,
                                             ComPtr<ID3D12DescriptorHeap> &descriptorHeap,
                                             D3D12_DESCRIPTOR_HEAP_FLAGS flags/* = D3D12_DESCRIPTOR_HEAP_FLAG_NONE*/, UINT nodeMask/*= 0u*/)
{
    D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
    heapDesc.Type = type;
    heapDesc.NumDescriptors = numDescriptors;
    heapDesc.Flags = (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : flags;
    heapDesc.NodeMask = nodeMask;
    return m_device->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(descriptorHeap.GetAddressOf()));
}

UINT Blainn::Device::GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE heapType)
{
    return m_device->GetDescriptorHandleIncrementSize(heapType);
}

HRESULT Blainn::Device::CreateGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC &psoDesc, ComPtr<ID3D12PipelineState> &pipelineState)
{
    return m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));
}

VOID Blainn::Device::CreateDepthStencilView(ID3D12Resource *pResource, const DXGI_FORMAT format , CD3DX12_CPU_DESCRIPTOR_HANDLE destDescriptor)
{
    D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
    dsvDesc.Format = format;
    dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
    dsvDesc.Texture2D.MipSlice = 0u;
    m_device->CreateDepthStencilView(pResource, &dsvDesc, destDescriptor);
}

VOID Blainn::Device::CreateRenderTargetView(ID3D12Resource *pResource, const D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc,
                                            CD3DX12_CPU_DESCRIPTOR_HANDLE destDescriptor)
{
    m_device->CreateRenderTargetView(pResource, rtvDesc, destDescriptor);
}

HRESULT Blainn::Device::CreateRootSignature(UINT nodeMask, const void* pBlobRootSignature, SIZE_T blobLengthBytes, ComPtr<ID3D12RootSignature>& rootSignature)
{
    return m_device->CreateRootSignature(nodeMask, pBlobRootSignature, blobLengthBytes, IID_PPV_ARGS(rootSignature.GetAddressOf()));
}

HRESULT Blainn::Device::CreateCommittedResource(const D3D12_HEAP_TYPE heapType, D3D12_HEAP_FLAGS heapFlags,
                                               const D3D12_RESOURCE_DESC& resourceDesc,
                                               D3D12_RESOURCE_STATES initialResourceState,
                                               const D3D12_CLEAR_VALUE &optClearValue, ComPtr<ID3D12Resource> &resource)
{
    auto heapProp = CD3DX12_HEAP_PROPERTIES(heapType);

    return m_device->CreateCommittedResource(&heapProp, heapFlags, &resourceDesc, initialResourceState, &optClearValue,
                                             IID_PPV_ARGS(resource.GetAddressOf()));
}
