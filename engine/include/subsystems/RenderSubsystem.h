#pragma once

#include <Windows.h>
#include "Render/DXHelpers.h"
#include "Render/SwapChain.h"
#include "Render/Device.h"

#include "handles/Handle.h"
#include "scene/Entity.h"
#include "Render/Camera.h"
#include "Render/GBuffer.h"
#include "Render/CascadeShadowMap.h"

namespace Blainn
{
const int gNumFrameResources = 3;

class Device;
class Renderer;
class RootSignature;
struct FrameResource;

class RenderSubsystem
{
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
        CascadedShadowsOpaque = 0,

        DeferredGeometry,
        Wireframe,

        DeferredDirectional,
        DeferredPointWithinFrustum,
        DeferredPointIntersectsFarPlane,
        DeferredPointFullQuad,
        DeferredSpot,

        Transparency,

        NumPipelineStates = 9u
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

    void Init(HWND window);
    void SetWindowParams(HWND window);
    void Render(float deltaTime);
    void OnResize(UINT newWidth, UINT newHeight);
    void Destroy();

    void CreateAttachRenderComponent(Entity entity);
    void AddMeshToRenderComponent(Entity entity, eastl::shared_ptr<MeshHandle> meshHandle);
    // Record all the commands we need to render the scene into the command list.
    void PopulateCommandList(ID3D12GraphicsCommandList2 *pCommandList);

private:
    void InitializeD3D();

#pragma region BoilerplateD3D12
    VOID GetHardwareAdapter(IDXGIFactory1 *pFactory, IDXGIAdapter1 **ppAdapter,
                            bool requestHighPerformanceAdapter = false);
    VOID SetCustomWindowText(LPCWSTR text) const;

    VOID CreateSwapChain();
    VOID CreateRtvAndDsvDescriptorHeaps();

    VOID Reset();
    VOID ResetGraphicsFeatures();

    VOID Present();
#pragma endregion BoilerplateD3D12

    void LoadPipeline();
    void LoadGraphicsFeatures();

    void CreateFrameResources();

    void CreateDescriptorHeaps();
    void CreateRootSignature();
    void CreateShaders();
    void CreatePipelineStateObjects();

private:
    void UpdateObjectsCB(float deltaTime);
    void UpdateMaterialBuffer(float deltaTime);
    void UpdateLightsBuffer(float deltaTime);
    void UpdateShadowTransform(float deltaTime);
    void UpdateShadowPassCB(float deltaTime);
    void UpdateGeometryPassCB(float deltaTime);
    void UpdateMainPassCB(float deltaTime);

private:
#pragma region Shadows
    void RenderDepthOnlyPass(ID3D12GraphicsCommandList2 *pCommandList);
#pragma endregion Shadows
#pragma region DeferredShading
    void RenderGeometryPass(ID3D12GraphicsCommandList2 *pCommandList);
    void RenderLightingPass(ID3D12GraphicsCommandList2 *pCommandList);
    void RenderTransparencyPass(ID3D12GraphicsCommandList2 *pCommandList);

    void DeferredDirectionalLightPass(ID3D12GraphicsCommandList2 *pCommandList);
    void DeferredPointLightPass(ID3D12GraphicsCommandList2 *pCommandList);
    void DeferredSpotLightPass(ID3D12GraphicsCommandList2 *pCommandList);
#pragma endregion DeferredShading

    void DrawMeshes(ID3D12GraphicsCommandList2 *cmdList);

    void DrawInstancedMeshes(ID3D12GraphicsCommandList2 *cmdList,
                             const eastl::vector<MeshData<BlainnVertex, uint32_t>> &meshData);

#pragma region CommandListIntrinsic
    void Draw(UINT vertexCount, UINT instanceCount = 1u, UINT startVertex = 0u, UINT startInstance = 0u);
    void DrawIndexed(UINT indexCount, UINT instanceCount = 1u, UINT startIndex = 0u, UINT baseVertex = 0u,
                     UINT startInstance = 0u);

#pragma region CommandListIntrinsic

    void DrawQuad(ID3D12GraphicsCommandList2 *pCommandList);

    eastl::pair<XMMATRIX, XMMATRIX> GetLightSpaceMatrix(const float nearZ, const float farZ);
    // Doubt that't a good idea to return vector of matrices. Should rather pass vector as a parameter probalby and
    // fill it inside function.
    void GetLightSpaceMatrices(eastl::vector<eastl::pair<XMMATRIX, XMMATRIX>> &outMatrices);

    eastl::vector<XMVECTOR> GetFrustumCornersWorldSpace(const XMMATRIX &view, const XMMATRIX &projection);

private:
    UINT m_dxgiFactoryFlags = 0u;

    UINT m_width;
    UINT m_height;
    float m_aspectRatio;

    HWND m_hWND;

    static inline bool m_isInitialized = false;
    bool m_areGraphicsFeaturesLoaded = false;
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

    UINT m_4xMsaaQuality = 0u;

private:
    // Pipeline objects.
    eastl::shared_ptr<SwapChain> m_swapChain;
    Device &m_device = Device::GetInstance();

    eastl::unique_ptr<Renderer> m_renderer = nullptr;

    ComPtr<ID3D12Resource> m_depthStencilBuffer;

    eastl::shared_ptr<RootSignature> m_rootSignature;
    eastl::unordered_map<EShaderType, ComPtr<ID3DBlob>> m_shaders;
    eastl::unordered_map<EPsoType, ComPtr<ID3D12PipelineState>> m_pipelineStates;

    // ObjectConstants m_perObjectCBData;

    float m_sunPhi = XM_PIDIV4;
    float m_sunTheta = 1.25f * XM_PI;

    PassConstants m_shadowPassCBData;
    PassConstants m_geometryPassCBData;
    PassConstants m_mainPassCBData; // deferred color(light) pass
    // PassConstants m_lightingPassCBData;
    MaterialData m_perMaterialSBData;
    InstanceData m_perInstanceSBData;

    ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
    ComPtr<ID3D12DescriptorHeap> m_dsvHeap;
    ComPtr<ID3D12DescriptorHeap> m_srvHeap;

    UINT m_rtvDescriptorSize;
    UINT m_dsvDescriptorSize;
    UINT m_cbvSrvUavDescriptorSize;

    D3D12_VIEWPORT m_viewport;
    D3D12_RECT m_scissorRect;

    int m_currFrameResourceIndex = 0;
    eastl::vector<eastl::unique_ptr<FrameResource>> m_frameResources;
    FrameResource *m_currFrameResource = nullptr;

    eastl::unique_ptr<Camera> m_camera;

#pragma region DeferredShading
    eastl::unique_ptr<GBuffer> m_GBuffer;
    CD3DX12_GPU_DESCRIPTOR_HANDLE m_GBufferTexturesSrv;
#pragma endregion DeferredShading

#pragma region CascadedShadows
    eastl::unique_ptr<ShadowMap> m_cascadeShadowMap;
    CD3DX12_GPU_DESCRIPTOR_HANDLE m_cascadeShadowSrv;
#pragma endregion CascadedShadows

private:
    D3D12_CPU_DESCRIPTOR_HANDLE GetRTV()
    {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(),
                                             m_swapChain->GetBackBufferIndex(), m_rtvDescriptorSize);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE GetDSV()
    {
        return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
    }
};
} // namespace Blainn