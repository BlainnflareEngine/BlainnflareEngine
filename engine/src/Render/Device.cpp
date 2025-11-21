#pragma once

#include "Render/Device.h"
#include "Render/CommandQueue.h"
#include "Render/DXHelpers.h"

//Blainn::Device::Device()
//    : m_useWarpDevice(false)
//{
//    // HRESULT check
//    HRESULT hr = (CreateDXGIFactory2(m_dxgiFactoryFlags, IID_PPV_ARGS(&m_factory)));
//    ThrowIfFailed(hr);
//
//    if (m_useWarpDevice)
//    {
//        ComPtr<IDXGIAdapter> warpAdapter;
//        // HRESULT check
//        ThrowIfFailed(m_factory->EnumWarpAdapter(IID_PPV_ARGS(&warpAdapter)));
//        // HRESULT check
//        ThrowIfFailed(D3D12CreateDevice(warpAdapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&m_device)));
//    }
//    else
//    {
//        GetHardwareAdapter(m_factory.Get(), &m_hardwareAdapter);
//        
//        // HRESULT check
//        ThrowIfFailed(D3D12CreateDevice(m_hardwareAdapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device)));
//    }
//}
//
//Blainn::Device::~Device()
//{
//}
//
//void Blainn::Device::CreateDebugLayer()
//{
//    ComPtr<ID3D12Debug> debugController;
//    if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController))))
//    {
//        debugController->EnableDebugLayer();
//
//        // Enable additional debug layers.
//        m_dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
//    }
//}
//
// Helper function for acquiring the first available hardware adapter that supports Direct3D 12.
// If no such adapter can be found, *ppAdapter will be set to nullptr.
//_Use_decl_annotations_ void Blainn::Device::GetHardwareAdapter(IDXGIFactory1 *pFactory, IDXGIAdapter1 **ppAdapter, bool requestHighPerformanceAdapter)
//{
//    *ppAdapter = nullptr;
//
//    ComPtr<IDXGIAdapter1> adapter;
//
//    ComPtr<IDXGIFactory6> factory6;
//    if (SUCCEEDED(pFactory->QueryInterface(IID_PPV_ARGS(&factory6))))
//    {
//        for (UINT adapterIndex = 0; SUCCEEDED(factory6->EnumAdapterByGpuPreference(
//                 adapterIndex,
//                 requestHighPerformanceAdapter == true ? DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE
//                                                       : DXGI_GPU_PREFERENCE_UNSPECIFIED,
//                 IID_PPV_ARGS(&adapter)));
//             ++adapterIndex)
//        {
//            DXGI_ADAPTER_DESC1 desc;
//            adapter->GetDesc1(&desc);
//
//            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
//            {
//                // Don't select the Basic Render Driver adapter.
//                // If you want a software adapter, pass in "/warp" on the command line.
//                continue;
//            }
//
//            // Check to see whether the adapter supports Direct3D 12, but don't create the
//            // actual device yet.
//            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
//            {
//                break;
//            }
//        }
//    }
//
//    if (adapter.Get() == nullptr)
//    {
//        for (UINT adapterIndex = 0; SUCCEEDED(pFactory->EnumAdapters1(adapterIndex, &adapter)); ++adapterIndex)
//        {
//            DXGI_ADAPTER_DESC1 desc;
//            adapter->GetDesc1(&desc);
//
//            if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
//            {
//                // Don't select the Basic Render Driver adapter.
//                // If you want a software adapter, pass in "/warp" on the command line.
//                continue;
//            }
//
//            // Check to see whether the adapter supports Direct3D 12, but don't create the
//            // actual device yet.
//            if (SUCCEEDED(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, _uuidof(ID3D12Device), nullptr)))
//            {
//                break;
//            }
//        }
//    }
//
//    *ppAdapter = adapter.Detach();
//}
//
//ID3D12CommandQueue* Blainn::Device::GetCommandQueue(ECommandQueueType queueType)
//{
//    return m_renderingCmdQueues[static_cast<int>(queueType)]->Get();
//}
//
//ComPtr<IDXGISwapChain4> Blainn::Device::CreateSwapChain(DXGI_SWAP_CHAIN_DESC1 &desc, DXGI_SWAP_CHAIN_FULLSCREEN_DESC &fullScreenDesc, HWND window)
//{
//    ComPtr<IDXGISwapChain4> swapChain;
//
//    ComPtr<IDXGISwapChain1> swapChain1;
//    ThrowIfFailed(m_factory->CreateSwapChainForHwnd(
//        GetCommandQueue(ECommandQueueType::GFX), // Swap chain needs the queue so that it can force a flush on it.
//        window,
//        &desc,
//        &fullScreenDesc,
//        nullptr,
//        &swapChain1));
//
//    // This sample does not support fullscreen transitions.
//    ThrowIfFailed(m_factory->MakeWindowAssociation(window, DXGI_MWA_NO_ALT_ENTER));
//
//    ThrowIfFailed(swapChain1.As(&swapChain));
//    
//    return swapChain;
//}
//
//static D3D12_COMMAND_LIST_TYPE QueueTypeToCommandListType(const Blainn::ECommandQueueType queueType)
//{
//    using namespace Blainn;
//
//    switch (queueType)
//    {
//    case ECommandQueueType::GFX:
//        return D3D12_COMMAND_LIST_TYPE_DIRECT;
//    case ECommandQueueType::COMPUTE:
//        return D3D12_COMMAND_LIST_TYPE_COMPUTE;
//    case ECommandQueueType::COPY:
//        return D3D12_COMMAND_LIST_TYPE_COPY;
//    case ECommandQueueType::NUM_COMMAND_QUEUE_TYPES:
//        break;
//    }
//    return D3D12_COMMAND_LIST_TYPE_DIRECT;
//}
//
//void Blainn::Device::CreateCommandQueues()
//{
//    auto device = eastl::shared_ptr<Device>(this);
//    for (UINT i = 0; i != static_cast<UINT>(ECommandQueueType::NUM_COMMAND_QUEUE_TYPES); ++i)
//    {
//        m_renderingCmdQueues[i] = eastl::make_shared<CommandQueue>(device, QueueTypeToCommandListType(static_cast<ECommandQueueType>(i)));
//    }
//}