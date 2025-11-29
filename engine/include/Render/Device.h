#pragma once

#include <Windows.h>
#include "DXHelpers.h"

struct ID3D12Device2;
struct IDXGIFactory4;
struct IDXGIAdapter1;

using namespace Microsoft::WRL;

namespace Blainn
{
    class SwapChain;
    class CommandQueue;

    class Device /*: public eastl::enable_shared_from_this<Device>*/
    {
    protected:
        Device() = default;
        
    public:
        void Init(bool bUseWarpAdapter = false);
        static Device& GetInstance();
        
        ~Device();
        
        // NOTE: Enabling the debug layer after device creation will invalidate the active device.
        static VOID CreateDebugLayer();

        ComPtr<ID3D12Device2> GetDevice2() const { return m_device; }
        ComPtr<IDXGIFactory4> GetFactory() const { return m_factory; }
        
        VOID Flush();

        eastl::shared_ptr<CommandQueue> GetCommandQueue(D3D12_COMMAND_LIST_TYPE commandListType = D3D12_COMMAND_LIST_TYPE_DIRECT) const;
        
        eastl::shared_ptr<SwapChain> CreateSwapChain(HWND window, DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R10G10B10A2_UNORM);
        VOID CreateCommandQueues();
        HRESULT CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE commandListType, ComPtr<ID3D12CommandAllocator>&  commandAllocator);
        
        HRESULT CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors,
                                     ComPtr<ID3D12DescriptorHeap> &descriptorHeap,
                                     D3D12_DESCRIPTOR_HEAP_FLAGS flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE,
                                     UINT nodeMask = 0u);
        UINT GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE heapType);
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
    };
}