#pragma once

#include "DXHelpers.h"

namespace Blainn
{
    class Device;
    class CommandQueue;

    class SwapChain
    {
    public:
        SwapChain(HWND hWnd);
        virtual ~SwapChain();

    private:
        void ResetRenderTargets();

    public:
        bool IsFullscreen() const { return m_bIsFullscreen; }
        void SetFullscreen(bool fullscreen);
        void ToggleFullscreen() { SetFullscreen(!m_bIsFullscreen); }

        void SetVSync(bool vSync) { m_bIsVSync = vSync; }
        bool GetVSync() const { return m_bIsVSync; }
        void ToggleVSync() { SetVSync(!m_bIsVSync); }
        void SetVSyncEnabled(bool value) {SetVSync(value);}

        bool IsTearingSupported() const { return m_bIsTearingSupported; }

        //void WaitForSwapChain();
        void Reset(UINT width, UINT height);

        VOID Present();
        ID3D12Resource* GetBackBuffer() const;
        UINT GetBackBufferIndex() const { return m_currBackBuffer; }

        ComPtr<IDXGISwapChain3> GetSwapChain() const { return m_dxgiSwapChain; }

    private:
        ComPtr<IDXGISwapChain3> m_dxgiSwapChain;

        ComPtr<ID3D12Resource> m_renderTargets[RenderCommon::kSwapChainBufferCount];
        UINT m_currBackBuffer = 0u;

        uint32_t m_width;
        uint32_t m_height;

        bool m_bIsVSync;
        bool m_bIsTearingSupported;
        bool m_bIsFullscreen;
    };
} // namespace Blainn