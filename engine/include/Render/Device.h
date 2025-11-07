#pragma once

#include "DXHelpers.h"

namespace Blainn
{
    class Device
    {
    public:
        Device();
        ~Device();

        // NOTE: Enabling the debug layer after device creation will invalidate the active device.
        static void CreateDebugLayer();

    private:
        void GetHardwareAdapter(_In_ IDXGIFactory1 *pFactory, _Outptr_result_maybenull_ IDXGIAdapter1 **ppAdapter, bool requestHighPerformanceAdapter = false);

    private:
        static inline UINT m_dxgiFactoryFlags = 0u;

        // Adapter info.
        bool m_useWarpDevice;

        ComPtr<ID3D12Device> m_device;
        ComPtr<IDXGIFactory4> m_factory;

        ComPtr<IDXGIAdapter1> m_hardwareAdapter;
    };
}