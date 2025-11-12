#pragma once

#include "Render/DXHelpers.h"
#include "Render/CommandQueue.h"
#include "Render/Device.h"
#include <DirectXTK12/Src/d3dx12.h>

using namespace Microsoft::WRL;

namespace Blainn
{
    class CommandQueue;
    class Device;
    
    class Renderer
    {
    public:
        Renderer() = default;
        Renderer(eastl::shared_ptr<Device> pDevice, uint32_t width, uint32_t height);
        ~Renderer();
        
        void Init();
        void CreateCommandObjects();
        
    public:
        /*virtual*/ void Update(float deltaTime) /*override*/;
        /*virtual*/ void RenderScene(void) /*override*/;

        void PopulateCommandList();
        void ExecuteCommandLists();

    public:
        inline ID3D12Device* GetDevicePtr() const { return m_device->GetDevicePtr(); }
        inline ID3D12CommandQueue* GetRenderCommandQueue() const { return m_device->GetCommandQueue(ECommandQueueType::GFX); } 
        
    private:
        void CreateRtvAndDsvDescriptorHeaps();
        void CreateRootSignature();
        void CreatePipelineStateObjects();
        void CreateShaders();

    private:
        // no ownership
        eastl::shared_ptr<Device> m_device;
        uint32_t m_width;
        uint32_t m_height;
        
        static inline const uint32_t SwapChainFrameCount = 2u;
        static inline const DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        static inline const DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

        ComPtr<ID3D12CommandAllocator> m_commandAllocators[SwapChainFrameCount];
        ComPtr<ID3D12GraphicsCommandList> m_commandList;

        ComPtr<ID3D12Resource> m_renderTargets[SwapChainFrameCount];
        ComPtr<ID3D12Resource> m_depthStencilBuffer;
        
        // Synchronization objects.
        uint32_t m_frameIndex = 0u; // keep track of front and back buffers (see SwapChainFrameCount)
        ComPtr<ID3D12Fence> m_fence;
        HANDLE m_fenceEvent;
        uint64_t m_fenceValues[SwapChainFrameCount];
    };  
}