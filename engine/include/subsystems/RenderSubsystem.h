#pragma once

#include <Windows.h>
#include "Render/DXHelpers.h"

namespace Blainn
{
    class RenderSubsystem
    {
    private:
        RenderSubsystem() = delete; 
        RenderSubsystem(const RenderSubsystem&) = delete;
        RenderSubsystem& operator=(const RenderSubsystem&) = delete; 
        RenderSubsystem(const RenderSubsystem&&) = delete;
        RenderSubsystem& operator=(const RenderSubsystem&&) = delete; 
    public:
        // TODO: virtual ~RenderSubsystem() override = default;
        static void Init();
        static void Destroy();

        static void Render();
        // TODO: Additional rendering-specific methods can be added here
        
    private:
    #pragma region GoingDirectX
        static void InitializeDirect3D();
    
        static void LoadPipeline();

        static void CreateDebugLayer();
        static void CreateDevice();
        static void CreateCommandObjects();
        static void CreateFence();
        static void CreateSwapChain();
        static void CreateRtvAndDsvDescriptorHeaps();

        static void CreateRootSignature();
        static void CreateShaders();   
        static void CreatePSO();

        static void OnResize(UINT newWidth, UINT newHeight);
        static void Reset();
        
        #pragma endregion GoingDirectX
        
    private:
        HWND m_hWND;
        static inline bool m_isInitialized = false;
        
        UINT m_dxgiFactoryFlags = 0u;

        UINT m_width = 0u;
        UINT m_height = 0u;

        float m_aspectRatio;

        static const UINT SwapChainFrameCount = 2;
        static const DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
        static const DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

        UINT m_rtvDescriptorSize;       // see m_rtvHeap
        UINT m_dsvDescriptorSize;       // see m_dsvHeap
        UINT m_cbvSrvUavDescriptorSize; // see m_cbvHeap

        // Pipeline objects.
        ComPtr<IDXGIFactory4> m_factory;
        ComPtr<IDXGISwapChain3> m_swapChain;
        ComPtr<ID3D12Device> m_device;

        ComPtr<IDXGIAdapter1> m_hardwareAdapter;

        ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
        ComPtr<ID3D12DescriptorHeap> m_cbvHeap;

        ComPtr<ID3D12CommandQueue> m_commandQueue;
        ComPtr<ID3D12CommandAllocator> m_commandAllocators[SwapChainFrameCount];
        ComPtr<ID3D12GraphicsCommandList> m_commandList;

        ComPtr<ID3D12Resource> m_renderTargets[SwapChainFrameCount];
        ComPtr<ID3D12Resource> m_depthStencilBuffer;

        // Synchronization objects.
        UINT m_frameIndex = 0u; // keep track of front and back buffers (see SwapChainFrameCount)
        ComPtr<ID3D12Fence> m_fence;
        HANDLE m_fenceEvent;
        UINT64 m_fenceValues[SwapChainFrameCount];

        D3D12_VIEWPORT m_viewport;
        D3D12_RECT m_scissorRect;
    };
} // namespace Blainn