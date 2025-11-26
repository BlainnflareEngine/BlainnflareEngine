#pragma once

#include "Render/DXHelpers.h"
#include "Render/FrameResource.h"
#include <Windows.h>

#include "Render/Camera.h"
#include "Render/CascadeShadowMap.h"
#include "Render/GBuffer.h"
#include "handles/Handle.h"
#include "scene/Entity.h"

namespace Blainn
{
const int gNumFrameResources = 3;

class RenderSubsystem
{
    friend class Renderer;

public:
    enum ERootParameter : UINT
    {
        PerObjectDataCB = 0,
        PerPassDataCB,
        MaterialDataSB,
        PointLightsDataSB,
        SpotLightsDataSB,
        CascadedShadowMaps,
        Textures,
        GBufferTextures,

        NumRootParameters = 8u
    };

    enum EPsoType : UINT
    {
        CascadedShadowsOpaque = 0u,

        DeferredGeometry,
        DeferredDirectional,

        DeferredPointWithinFrustum,
        DeferredPointIntersectsFarPlane,
        DeferredPointFullQuad,

        DeferredSpot,

        NumPipelineStates = 7u
    };

    enum EShaderType : UINT
    {
        CascadedShadowsVS = 0u,
        CascadedShadowsGS,

        DeferredGeometryVS,
        DeferredGeometryPS,
        DeferredDirVS,
        DeferredDirPS,
        DeferredLightVolumesVS,
        DeferredPointPS,
        DeferredSpotPS,

        NumShaders = 9U
    };

private:
    RenderSubsystem() = default;
    RenderSubsystem(const RenderSubsystem &) = delete;
    RenderSubsystem &operator=(const RenderSubsystem &) = delete;
    RenderSubsystem(const RenderSubsystem &&) = delete;
    RenderSubsystem &operator=(const RenderSubsystem &&) = delete;

public:
    static RenderSubsystem &GetInstance();

    void Init(HWND windowHandle);
    void Render(float deltaTime);
    void Destroy();

    void CreateAttachRenderComponent(Entity entity);
    void AddMeshToRenderComponent(Entity entity, eastl::shared_ptr<Blainn::MeshHandle> meshHandle);

    void PopulateCommandList();

private:
    void InitializeD3D();

#pragma region BoilerplateD3D12
    VOID CreateDebugLayer();
    VOID GetHardwareAdapter(IDXGIFactory1 *pFactory, IDXGIAdapter1 **ppAdapter,
                            bool requestHighPerformanceAdapter = false);
    VOID SetCustomWindowText(LPCWSTR text) const;
    VOID CreateDevice();
    VOID CreateCommandObjects();
    VOID CreateFence();
    VOID CreateRtvAndDsvDescriptorHeaps();
    VOID CreateSwapChain();
    VOID WaitForGPU();
    VOID MoveToNextFrame();
    VOID Reset();
#pragma endregion BoilerplateD3D12
    void OnResize(UINT newWidth, UINT newHeight);

    void LoadPipeline();

    void CreateFrameResources();
    void CreateDescriptorHeaps();
    void CreateRootSignature();
    void CreateShaders();
    void CreatePipelineStateObjects();

public:
    // Engine(UINT width, UINT height, const std::wstring& name, const std::wstring& className);

    // virtual void OnUpdate(float deltaTime);
    // virtual void OnRender(float deltaTime);
    // virtual void OnDestroy();

    // virtual void OnMouseDown(WPARAM btnState, int x, int y) override;
    // virtual void OnMouseUp(WPARAM btnState, int x, int y) override;
    // virtual void OnMouseMove(WPARAM btnState, int x, int y) override;
    // virtual void OnKeyDown(UINT8 key) override;
    // virtual void OnKeyUp(UINT8 key) override;

private:
    void OnKeyboardInput(float deltaTime);
    void UpdateObjectsCB(float deltaTime);
    void UpdateMaterialBuffer(float deltaTime);
    void UpdateLightsBuffer(float deltaTime);
    void UpdateShadowTransform(float deltaTime);
    void UpdateShadowPassCB(float deltaTime);
    void UpdateGeometryPassCB(float deltaTime);
    void UpdateMainPassCB(float deltaTime);

private:
#pragma region Shadows
    void RenderDepthOnlyPass();
#pragma endregion Shadows
#pragma region DeferredShading
    void RenderGeometryPass();
    void RenderLightingPass();
    void RenderTransparencyPass();

    void DeferredDirectionalLightPass();
    void DeferredPointLightPass();
    void DeferredSpotLightPass();
#pragma endregion DeferredShading

    // void DrawRenderItems(ID3D12GraphicsCommandList* cmdList, std::vector<std::unique_ptr<RenderItem>>& renderItems);
    //  void DrawInstancedRenderItems(ID3D12GraphicsCommandList* cmdList, std::vector<std::unique_ptr<RenderItem>>&
    //  renderItems);
    void DrawQuad(ID3D12GraphicsCommandList *cmdList);

    eastl::array<const CD3DX12_STATIC_SAMPLER_DESC, 5> GetStaticSamplers();

    eastl::pair<XMMATRIX, XMMATRIX> GetLightSpaceMatrix(const float nearZ, const float farZ);
    // Doubt that't a good idea to return vector of matrices. Should rather pass vector as a parameter probalby and
    // fill it inside function.
    void GetLightSpaceMatrices(eastl::vector<eastl::pair<XMMATRIX, XMMATRIX>> &outMatrices);
    void CreateShadowCascadeSplits();

    eastl::vector<XMVECTOR> GetFrustumCornersWorldSpace(const XMMATRIX &view, const XMMATRIX &projection);

private:
    UINT m_dxgiFactoryFlags = 0u;

    UINT m_width;
    UINT m_height;
    float m_aspectRatio;

    HWND m_hWND;

    static inline bool m_isInitialized = false;
    bool m_useWarpDevice = false;

    static inline const uint32_t SwapChainFrameCount = 2u;
    static inline const DXGI_FORMAT BackBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    static inline const DXGI_FORMAT DepthStencilFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

    bool m_appPaused = false;       // is the application paused ?
    bool m_minimized = false;       // is the application minimized ?
    bool m_maximized = false;       // is the application maximized ?
    bool m_resizing = false;        // are the resize bars being dragged ?
    bool m_fullscreenState = false; // fullscreen enabled
    bool m_isWireframe = false;     // Fill mode
    bool m_is4xMsaaState = false;

private:
    // Pipeline objects.
    ComPtr<IDXGIFactory4> m_factory;
    ComPtr<IDXGISwapChain3> m_swapChain;
    ComPtr<ID3D12Device> m_device;
    ComPtr<IDXGIAdapter1> m_hardwareAdapter;

    eastl::unique_ptr<Renderer> m_renderer = nullptr;

    ComPtr<ID3D12CommandQueue> m_commandQueue;
    ComPtr<ID3D12CommandAllocator> m_commandAllocators[SwapChainFrameCount];
    ComPtr<ID3D12GraphicsCommandList> m_commandList;

    ComPtr<ID3D12Resource> m_renderTargets[SwapChainFrameCount];
    ComPtr<ID3D12Resource> m_depthStencilBuffer;

    UINT m_4xMsaaQuality = 0u;

    // Synchronization objects.
    UINT m_frameIndex = 0u; // keep track of front and back buffers (see SwapChainFrameCount)
    ComPtr<ID3D12Fence> m_fence;
    HANDLE m_fenceEvent;
    UINT64 m_fenceValues[SwapChainFrameCount];

    ComPtr<ID3D12RootSignature> m_rootSignature;
    eastl::unordered_map<EShaderType, ComPtr<ID3DBlob>> m_shaders;
    eastl::unordered_map<EPsoType, ComPtr<ID3D12PipelineState>> m_pipelineStates;

    ObjectConstants m_perObjectCBData;
    PassConstants m_shadowPassCBData;
    PassConstants m_geometryPassCBData;
    PassConstants m_mainPassCBData; // deferred color(light) pass
    // PassConstants m_lightingPassCBData;
    MaterialData m_perMaterialSBData;
    InstanceData m_perInstanceSBData;

    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
    ComPtr<ID3D12DescriptorHeap> m_srvHeap;

    UINT m_rtvDescriptorSize;       // see m_rtvHeap
    UINT m_dsvDescriptorSize;       // see m_dsvHeap
    UINT m_cbvSrvUavDescriptorSize; // see m_cbvHeap

    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;

    int m_currFrameResourceIndex = 0;
    eastl::vector<eastl::unique_ptr<FrameResource>> m_frameResources;
    FrameResource *m_currFrameResource = nullptr;

    eastl::unique_ptr<Camera> m_camera;

#pragma region DeferredShading
    eastl::unique_ptr<GBuffer> m_GBuffer;
#pragma endregion DeferredShading

#pragma region CascadedShadows
    CD3DX12_GPU_DESCRIPTOR_HANDLE m_cascadeShadowSrv;
    CD3DX12_GPU_DESCRIPTOR_HANDLE m_GBufferTexturesSrv;
    float m_shadowCascadeLevels[MaxCascades] = {0.0f, 0.0f, 0.0f, 0.0f};
#pragma endregion CascadedShadows

private:
    D3D12_CPU_DESCRIPTOR_HANDLE GetRTV()
    {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_frameIndex,
                                             m_rtvDescriptorSize);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE GetDSV()
    {
        return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
    }
};
} // namespace Blainn