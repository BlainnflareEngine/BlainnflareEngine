#pragma once

#include "DXHelpers.h"

namespace Blainn
{
    class Device;
    class CommandQueue;

    class SwapChain
    {
    public:
        SwapChain(Device& device, HWND hWnd, DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R10G10B10A2_UNORM);
        virtual ~SwapChain();

    private:
        void ResetRenderTargets(CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle, UINT rtvDescriptorSize);
     
    public:
        static const UINT SwapChainFrameCount = 2u;

        bool IsFullscreen() const { return m_fullscreen; }
        void SetFullscreen(bool fullscreen);
        void ToggleFullscreen() { SetFullscreen(!m_fullscreen); }

        void SetVSync(bool vSync) { m_vsync = vSync; }
        bool GetVSync() const { return m_vsync; }
        void ToggleVSync() { SetVSync(!m_vsync); }

        bool IsTearingSupported() const { return m_tearingSupported; }

        //void WaitForSwapChain();
        void Reset(UINT width, UINT height, CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle, UINT rtvDescriptorSize);

        VOID Present();
        ID3D12Resource* GetBackBuffer() const;
        UINT GetBackBufferIndex() const { return m_currBackBuffer; }
        DXGI_FORMAT GetBackBufferFormat() const { return m_backBufferFormat; }

        Microsoft::WRL::ComPtr<IDXGISwapChain3> GetSwapChain() const { return m_dxgiSwapChain; }

    private:
        Microsoft::WRL::ComPtr<IDXGISwapChain3> m_dxgiSwapChain;

        Device& m_device;                               // Device that created this swap chain

        ComPtr<ID3D12Resource> m_renderTargets[SwapChainFrameCount];
        UINT m_currBackBuffer = 0u;
        DXGI_FORMAT m_backBufferFormat;

        HWND m_hWnd;

        uint32_t m_width;
        uint32_t m_height;

        bool m_vsync;
        bool m_tearingSupported;
        bool m_fullscreen;
    };
} // namespace Blainn