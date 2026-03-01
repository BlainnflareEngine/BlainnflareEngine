#pragma once
#include <EASTL/shared_ptr.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <DirectXTK12/Src/d3dx12.h>

#include "DXHelpers.h"

struct ID3D12Device2;
struct IDXGIFactory4;
struct IDXGIAdapter1;

using namespace Microsoft::WRL;

namespace Blainn
{
    class SwapChain;
    class CommandQueue;

    class Device
    {
        Device() = default;
        
    public:
        static Device& GetInstance();
        void Init(bool bUseWarpAdapter = false);
        void Destroy();

        // NOTE: Enabling the debug layer after device creation will invalidate the active device.
        static VOID CreateDebugLayer();

        ComPtr<ID3D12Device2> GetDevice2() const { return m_device; }
        ComPtr<IDXGIFactory4> GetFactory() const { return m_factory; }
        ComPtr<IDXGIAdapter1> GetDXGIAdapter() const { return m_hardwareAdapter; }

        VOID Flush();

        eastl::shared_ptr<CommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE commandListType = D3D12_COMMAND_LIST_TYPE_DIRECT) const;
        
        eastl::shared_ptr<SwapChain> CreateSwapChain(HWND window, DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R10G10B10A2_UNORM);
        HRESULT CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE commandListType, ComPtr<ID3D12CommandAllocator>&  commandAllocator);
        
        HRESULT CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors,
                                     D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
                                     UINT nodeMask = 0u);

    private:
        HRESULT CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors,
                                     ComPtr<ID3D12DescriptorHeap> &descriptorHeap,
                                     D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
                                     UINT nodeMask = 0u);
        
    public:
        UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE heapType = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) const;

        HRESULT CreateGraphicsPipelineState(const D3D12_GRAPHICS_PIPELINE_STATE_DESC& psoDesc, ComPtr<ID3D12PipelineState>& pipelineState);
        
        VOID CreateDepthStencilView(ID3D12Resource *pResource, /*probably should pass whole desc*/ const DXGI_FORMAT format,
                                    CD3DX12_CPU_DESCRIPTOR_HANDLE destDescriptor);
        VOID CreateRenderTargetView(ID3D12Resource *pResource, const D3D12_RENDER_TARGET_VIEW_DESC* rtvDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE destDescriptor);

        VOID CreateShaderResourceView(ID3D12Resource *pResource, const D3D12_SHADER_RESOURCE_VIEW_DESC* srvDesc, CD3DX12_CPU_DESCRIPTOR_HANDLE destDescriptor);

        HRESULT CreateRootSignature(UINT nodeMask, const void *pBlobRootSignature, SIZE_T blobLengthBytes, ComPtr<ID3D12RootSignature> &rootSignature);


        HRESULT CreateCommittedResource(const D3D12_HEAP_TYPE heapType, D3D12_HEAP_FLAGS heapFlags,
                                       const D3D12_RESOURCE_DESC& resourceDesc,
                                       D3D12_RESOURCE_STATES initialResourceState,
                                       const D3D12_CLEAR_VALUE &optClearValue, ComPtr<ID3D12Resource>& resource);

        VOID CreateCommandQueues();
        bool IsInitialized() const { return m_isInitialized; }

    public:

        ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) const;

    private:
        VOID GetHardwareAdapter(_In_ IDXGIFactory1 *pFactory, _Outptr_result_maybenull_ IDXGIAdapter1 **ppAdapter, bool requestHighPerformanceAdapter = false);

    private:
        static inline UINT m_dxgiFactoryFlags = 0u;
        bool m_useWarpDevice = false;

        eastl::shared_ptr<CommandQueue> m_directCommandQueue;
        eastl::shared_ptr<CommandQueue> m_copyCommandQueue;
        eastl::shared_ptr<CommandQueue> m_computeCommandQueue;

        // Adapter info.
        ComPtr<IDXGIAdapter1> m_hardwareAdapter;

        ComPtr<ID3D12Device2> m_device = nullptr;
        //ComPtr<ID3D12Device4> m_device4 = nullptr;
        ComPtr<IDXGIFactory4> m_factory = nullptr;

        ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
        ComPtr<ID3D12DescriptorHeap> m_srvHeap;
        
        UINT m_rtvDescriptorSize;
        UINT m_dsvDescriptorSize;
        UINT m_cbvSrvUavDescriptorSize;

        bool m_isInitialized = false;
    };
}
