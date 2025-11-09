#pragma once

#include "DXHelpers.h"

struct ID3D12Device;
struct ID3D12Device4;
struct IDXGIFactory4;
struct IDXGIAdapter1;

namespace Blainn
{
    class Device
    {
    public:
        Device();
        ~Device();

        void Create(/* some description would be good */);
        void Destroy();

        // NOTE: Enabling the debug layer after device creation will invalidate the active device.
        static void CreateDebugLayer();

        inline ID3D12Device* GetDevicePtr() { return m_device.Get();}
        inline ID3D12Device* GetDevice4Ptr() { return m_device4.Get(); }
        inline IDXGIAdapter1* GetAdapter1Ptr() { return m_hardwareAdapter.Get(); }
    private:
        void GetHardwareAdapter(_In_ IDXGIFactory1 *pFactory, _Outptr_result_maybenull_ IDXGIAdapter1 **ppAdapter, bool requestHighPerformanceAdapter = false);

    private:
        static inline UINT m_dxgiFactoryFlags = 0u;

        // Adapter info.
        bool m_useWarpDevice = false;
        ComPtr<IDXGIAdapter1> m_hardwareAdapter;

        ComPtr<ID3D12Device> m_device = nullptr;
        ComPtr<ID3D12Device4> m_device4 = nullptr;
        ComPtr<IDXGIFactory4> m_factory = nullptr;
    };
}