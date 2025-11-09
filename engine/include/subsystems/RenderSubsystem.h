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
        void Init();
        void Render(float deltaTime);
        void Destroy();

    private:
    #pragma region GoingDirectX
        static void InitializeDirect3D();
    
        static void LoadPipeline();

        static void CreateRootSignature();
        static void CreateShaders();   
        static void CreatePSO();

        static void OnResize(UINT newWidth, UINT newHeight);
        static void Reset();
        
        #pragma endregion GoingDirectX
        
    private:
        HWND m_hWND;
        static inline bool m_isInitialized = false;

        eastl::unique_ptr<Renderer> m_renderer = nullptr;
        eastl::unique_ptr<Device> m_device = nullptr;
        
        UINT m_width = 0u;
        UINT m_height = 0u;

        float m_aspectRatio;

        UINT m_rtvDescriptorSize;       // see m_rtvHeap
        UINT m_dsvDescriptorSize;       // see m_dsvHeap
        UINT m_cbvSrvUavDescriptorSize; // see m_cbvHeap

        ComPtr<IDXGISwapChain3> m_swapChain;

        ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
        ComPtr<ID3D12DescriptorHeap> m_cbvHeap;

        D3D12_VIEWPORT m_viewport;
        D3D12_RECT m_scissorRect;
    };
} // namespace Blainn