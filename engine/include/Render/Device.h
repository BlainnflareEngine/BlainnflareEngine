#pragma once

#include <Windows.h>
#include "DXHelpers.h"

struct ID3D12Device;
struct ID3D12Device4;
struct IDXGIFactory4;
struct IDXGIAdapter1;

using namespace Microsoft::WRL;

namespace Blainn
{
    class CommandQueue;

    enum class ECommandQueueType
    {
        GFX = 0,
        COMPUTE,
        COPY,

        NUM_COMMAND_QUEUE_TYPES = 3
    };

    class Device /*: public eastl::enable_shared_from_this<Device>*/
    {
    public:
        Device();
        ~Device();

        // NOTE: Enabling the debug layer after device creation will invalidate the active device.
        static void CreateDebugLayer();
        ComPtr<IDXGISwapChain4> CreateSwapChain(DXGI_SWAP_CHAIN_DESC1 &desc, DXGI_SWAP_CHAIN_FULLSCREEN_DESC &fullScreenDesc, HWND window);
        void CreateCommandQueues();

        inline ID3D12Device* GetDevicePtr() { return m_device.Get();}
        inline ID3D12Device4* GetDevice4Ptr() { return m_device4.Get(); }
        inline IDXGIAdapter1* GetAdapter1Ptr() { return m_hardwareAdapter.Get(); }

        ID3D12CommandQueue* GetCommandQueue(ECommandQueueType queueType);

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

        // command queue should be associated with its fence object
        eastl::shared_ptr<CommandQueue> m_renderingCmdQueues[static_cast<int>(ECommandQueueType::NUM_COMMAND_QUEUE_TYPES)];
    };
}