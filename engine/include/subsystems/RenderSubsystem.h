#pragma once

#include <Windows.h>
#include "Render/DXHelpers.h"
#include <EASTL/unique_ptr.h>

namespace Blainn
{
    class Renderer;
    class Device;

    class RenderSubsystem
    {
    private:
        RenderSubsystem() = default; 
        RenderSubsystem(const RenderSubsystem&) = delete;
        RenderSubsystem& operator=(const RenderSubsystem&) = delete; 
        RenderSubsystem(const RenderSubsystem&&) = delete;
        RenderSubsystem& operator=(const RenderSubsystem&&) = delete; 
    public:
        static RenderSubsystem &GetInstance();
        void Init(HWND windowHandle);
        void Render(float deltaTime);
        void Destroy();
        
    private:
        void CreateDeviceResources();
        void CreateSwapChain();
        void CreateFence();
        void WaitForGPU();
        void MoveToNextFrame();
        
        static void LoadPipeline();

        static void CreateRootSignature();
        static void CreateShaders();

        static void OnResize(UINT newWidth, UINT newHeight);
        static void Reset();
        
    private:
        HWND m_hWND;
        static inline bool m_isInitialized = false;

        static inline const uint32_t SwapChainFrameCount = 2u;
        static inline const DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        static inline const DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

        eastl::unique_ptr<Renderer> m_renderer = nullptr;
        eastl::shared_ptr<Device> m_device = nullptr;
        ComPtr<IDXGISwapChain3> m_swapChain;

        // Synchronization objects.
        UINT m_frameIndex = 0u; // keep track of front and back buffers (see SwapChainFrameCount)
        ComPtr<ID3D12Fence> m_fence;
        HANDLE m_fenceEvent;
        UINT64 m_fenceValues[SwapChainFrameCount];
        
        UINT m_width = 0u;
        UINT m_height = 0u;

        float m_aspectRatio;

        UINT m_rtvDescriptorSize;       // see m_rtvHeap
        UINT m_dsvDescriptorSize;       // see m_dsvHeap
        UINT m_cbvSrvUavDescriptorSize; // see m_cbvHeap

        ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
        ComPtr<ID3D12DescriptorHeap> m_cbvHeap;

        D3D12_VIEWPORT m_viewport;
        D3D12_RECT m_scissorRect;
    };
} // namespace Blainn