#pragma once

#include "Render/Device.h"
#include "Render/SwapChain.h"

#include "handles/Handle.h"
#include "scene/Entity.h"
#include "Render/Camera.h"
#include "Render/GBuffer.h"
#include "Render/CascadeShadowMap.h"
#include "Render/RootSignature.h"
#include "Render/Shader.h"
#include "Render/PipelineStateObject.h"

namespace Blainn
{
const int gNumFrameResources = 3;

class Device;
class RootSignature;
struct FrameResource;
class SelectionManager;

class RenderSubsystem
{
private:
    RenderSubsystem() = default;
    RenderSubsystem(const RenderSubsystem &) = delete;
    RenderSubsystem &operator=(const RenderSubsystem &) = delete;
    RenderSubsystem(const RenderSubsystem &&) = delete;
    RenderSubsystem &operator=(const RenderSubsystem &&) = delete;

public:
    static RenderSubsystem &GetInstance();

    void PreInit();
    void Init(HWND window);
    void SetWindowParams(HWND window);
    void Render(float deltaTime);
    void OnResize(UINT newWidth, UINT newHeight);
    void Destroy();

public:
    void ToggleVSync()
    {
        if (!m_swapChain) return;
        m_swapChain->ToggleVSync();
    }

    void ToggleFullscreen()
    {
        if (!m_swapChain) return;
        m_swapChain->ToggleFullscreen();
    }

    void SetCamera(Camera* camera) { m_camera = camera; }
    Camera* GetCamera() { return m_camera; }
    eastl::shared_ptr<Camera>& GetEditorCamera() { return m_editorCamera; }

    float GetAspectRatio() const { return m_aspectRatio; }

private:
    // Record all the commands we need to render the scene into the command list.
    void PopulateCommandList(ID3D12GraphicsCommandList2 *pCommandList);
    void InitializeWindow();

#pragma region BoilerplateD3D12
    VOID GetHardwareAdapter(IDXGIFactory1 *pFactory, IDXGIAdapter1 **ppAdapter, bool requestHighPerformanceAdapter = false);
    VOID SetCustomWindowText(LPCWSTR text) const;

    VOID CreateSwapChain();
    VOID CreateDescriptorHeaps();

    VOID Reset();
    VOID ResetGraphicsFeatures();

    VOID Present();
#pragma endregion BoilerplateD3D12

    void LoadPipeline();
    void LoadGraphicsFeatures();
    void CreateFrameResources();
    void LoadInitTimeTextures(ID3D12GraphicsCommandList2* pCommandList);
    void LoadSrvAndSamplerDescriptorHeaps();
    void CreateRootSignature();
    void CreateShaders();
    void CreatePipelineStateObjects();

private:
    void UpdateObjectsCB(float deltaTime);
    void UpdateMaterialBuffer(float deltaTime);
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
    void DeferredDirectionalLightPass(ID3D12GraphicsCommandList2 *pCommandList);
    void DeferredPointLightPass(ID3D12GraphicsCommandList2 *pCommandList);
    void DeferredSpotLightPass(ID3D12GraphicsCommandList2 *pCommandList);

    void RenderForwardPasses(ID3D12GraphicsCommandList2 *pCommandList);
    void RenderTransparencyPass(ID3D12GraphicsCommandList2 *pCommandList);
#pragma endregion DeferredShading
    void RenderSkyBoxPass(ID3D12GraphicsCommandList2 *pCommandList);

    void ResourceBarrier(ID3D12GraphicsCommandList2 *pCommandList, ID3D12Resource* pResource, D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);

    // For drawing specific meshes
    void DrawMesh(ID3D12GraphicsCommandList2 *pCommandList, eastl::unique_ptr<struct MeshComponent>& mesh); 
    void DrawMeshes(ID3D12GraphicsCommandList2 *pCommandList);
    void DrawInstancedMeshes(ID3D12GraphicsCommandList2 *pCommandList, const eastl::vector<MeshData<BlainnVertex, uint32_t>> &meshData);

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

    ComPtr<ID3D12Resource> m_depthStencilBuffer;

    eastl::shared_ptr<RootSignature> m_rootSignature;
    eastl::unordered_map<Shader::EShaderType, ComPtr<ID3DBlob>> m_shaders;
    eastl::unordered_map<PipelineStateObject::EPsoType, ComPtr<ID3D12PipelineState>> m_pipelineStates;

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

    Camera* m_camera;
    eastl::shared_ptr<Camera> m_editorCamera;

#pragma region DeferredShading
    eastl::unique_ptr<GBuffer> m_GBuffer;
    UINT m_GBufferTexturesSrvHeapStartIndex = 0u;
#pragma endregion DeferredShading

#pragma region CascadedShadows
    eastl::unique_ptr<ShadowMap> m_cascadeShadowMap;
    UINT m_cascadesShadowSrvHeapStartIndex = 0u;
#pragma endregion CascadedShadows

#pragma region Textures
    UINT m_skyCubeSrvHeapStartIndex = 0u;
    UINT m_texturesSrvHeapStartIndex = 0u;
#pragma endregion Textures
    
    // TODO
    eastl::unique_ptr<struct MeshComponent> skyBox = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> skyBoxResource = nullptr;
    Microsoft::WRL::ComPtr<ID3D12Resource> skyBoxUploadHeap = nullptr;

    eastl::unique_ptr<SelectionManager> m_selectionManager = nullptr;

private:
    D3D12_CPU_DESCRIPTOR_HANDLE GetRTV()
    {
        return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_rtvHeap->GetCPUDescriptorHandleForHeapStart(), m_swapChain->GetBackBufferIndex(), m_rtvDescriptorSize);
    }

    D3D12_CPU_DESCRIPTOR_HANDLE GetDSV()
    {
        return m_dsvHeap->GetCPUDescriptorHandleForHeapStart();
    }
};
} // namespace Blainn