#pragma once

#include "DXHelpers.h"

namespace Blainn
{
    class Device;
    class CommandQueue;

    class SwapChain
    {
    public:
        struct Extent
        {
            UINT width = 0u;
            UINT height = 0u;
        };

        SwapChain(HWND hWnd, DXGI_FORMAT backBufferFormat = DXGI_FORMAT_R10G10B10A2_UNORM);
        virtual ~SwapChain();

    private:
        void ResetRenderTargets();

    public:
        static const UINT SwapChainFrameCount = 2u;

        bool IsFullscreen() const { return m_bIsFullscreen; }
        void SetFullscreen(bool fullscreen);
        void ToggleFullscreen() { SetFullscreen(!m_bIsFullscreen); }

        void SetVSync(bool vSync) { m_bIsVSync = vSync; }
        bool GetVSync() const { return m_bIsVSync; }
        void ToggleVSync() { SetVSync(!m_bIsVSync); }
        void SetVSyncEnabled(bool value) {SetVSync(value);}

        bool IsTearingSupported() const { return m_bIsTearingSupported; }

        //void WaitForSwapChain();
        void Reset(const Extent &extent);

        VOID Present();
        ID3D12Resource* GetBackBuffer() const;
        UINT GetBackBufferIndex() const { return m_currBackBuffer; }
        DXGI_FORMAT GetBackBufferFormat() const { return m_backBufferFormat; }

        Microsoft::WRL::ComPtr<IDXGISwapChain3> GetSwapChain() const { return m_dxgiSwapChain; }

    private:
        Microsoft::WRL::ComPtr<IDXGISwapChain3> m_dxgiSwapChain;

        ComPtr<ID3D12Resource> m_renderTargets[SwapChainFrameCount];
        UINT m_currBackBuffer = 0u;
        DXGI_FORMAT m_backBufferFormat;

        HWND m_hWnd;

        uint32_t m_width;
        uint32_t m_height;

        bool m_bIsVSync;
        bool m_bIsTearingSupported;
        bool m_bIsFullscreen;
    };
} // namespace Blainn
