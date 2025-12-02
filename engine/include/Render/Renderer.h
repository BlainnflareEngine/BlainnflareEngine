#pragma once

#include "Render/DXHelpers.h"

using namespace Microsoft::WRL;

namespace Blainn
{
    class Renderer
    {
    public:
        Renderer() = default;
        Renderer(ID3D12Device* device, uint32_t width, uint32_t height);
        ~Renderer();
        
        void Init();
        void CreateCommandObjects();
        
    public:
        void Update(float deltaTime);
        void RenderScene(void);

        void PopulateCommandList();
        void ExecuteCommandLists();
        
    private:
        void CreateRtvAndDsvDescriptorHeaps();
        void CreateRootSignature();
        void CreatePipelineStateObjects();
        void CreateShaders();

    private:
        // no ownership
        ID3D12Device* m_device = nullptr;

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
        uint32_t m_currBackBuffer = 0u; // keep track of front and back buffers (see SwapChainFrameCount)
        ComPtr<ID3D12Fence> m_fence;
        HANDLE m_fenceEvent;
        uint64_t m_fenceValues[SwapChainFrameCount];
    };  
}