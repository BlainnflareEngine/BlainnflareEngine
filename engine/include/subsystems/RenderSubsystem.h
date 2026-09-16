#pragma once

#include "handles/Handle.h"
#include "scene/Entity.h"
#include "Render/Camera.h"
#include "Render/Shader.h"
#include "Render/PipelineStateObject.h"
#include "Render/RenderTarget.h"

namespace Blainn
{
struct FrameResource;

class Device;
class SwapChain;
class RootSignature;
class GBuffer;
class ShadowMap;
class DebugRenderer;
class UIRenderer;

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

    void DestroyCameraComponent(Entity entity);
    void DestroyDirectionalLightComponent(Entity entity);
    void DestroyPointLightComponent(Entity entity);
    void DestroySpotLightComponent(Entity entity);
    void DestroySkyboxComponent(Entity entity);

public:
    void ToggleVSync();
    void SetVSyncEnabled(bool value);
    bool GetVSyncEnabled() const;
    void ToggleFullscreen();

    void SetEnableDebug(bool newValue);

    bool DebugEnabled() const
    {
        return m_enableDebugLayer;
    }

    const DebugRenderer &GetDebugRenderer() const
    {
        return *m_debugRenderer;
    }
    DebugRenderer &GetDebugRenderer()
    {
        return *m_debugRenderer;
    }

    const UIRenderer &GetUIRenderer() const
    {
        return *m_UIRenderer;
    }
    UIRenderer &GetUIRenderer()
    {
        return *m_UIRenderer;
    }

    void SetCamera(Camera *camera)
    {
        m_camera = camera;
    }
    Camera *GetCamera()
    {
        return m_camera;
    }
    eastl::shared_ptr<Camera> &GetEditorCamera()
    {
        return m_editorCamera;
    }

    float GetAspectRatio() const
    {
        return m_aspectRatio;
    }

    uuid GetUUIDAt(uint32_t x, uint32_t y);

private:
    // Record all the commands we need to render the scene into the command list.
    void PopulateCommandList(ID3D12GraphicsCommandList2 *pCommandList);
    void InitializeWindow();
    void InitializeImGui();

#pragma region BoilerplateD3D12
    VOID GetHardwareAdapter(IDXGIFactory1 *pFactory, IDXGIAdapter1 **ppAdapter, bool requestHighPerformanceAdapter = false);

    VOID CreateSwapChain();
    VOID CreateDescriptorHeaps();

    VOID Reset();
    VOID ResetGraphicsFeatures();

    VOID Present();
#pragma endregion BoilerplateD3D12

    void LoadPipeline();
    void LoadGraphicsFeatures();
    void CreateFrameResources();
    void LoadInitTimeTextures(ID3D12GraphicsCommandList2 *pCommandList);
    void LoadSrvAndSamplerDescriptorHeaps();
    void CreateRootSignature();
    void CreateShaders();
    void CreatePipelineStateObjects();

private:
    void UpdateObjectsCB(float deltaTime);
    void UpdateLightsBuffers(float deltaTime);
    void UpdateMaterialBuffer(float deltaTime);
    void UpdateShadowTransform(float deltaTime);

    void UpdateShadowPassCB(float deltaTime);
    void UpdateCommonRenderingData(float deltaTime);
    void UpdateGeometryPassCB(/*float deltaTime*/);
    void UpdateDeferredPassCB(/*float deltaTime*/);
    // Maybe we'll need this later but for now all the forward render data is alrealy in the const buffer
    void UpdateForwardPassCB(float deltaTime);

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

    void RenderDebugPass(ID3D12GraphicsCommandList2 *pCommandList);

    void RenderUUIDPass(ID3D12GraphicsCommandList2 *pCommandList);
    void RenderImGuiPass(ID3D12GraphicsCommandList2 *pCommandList);

    void ResourceBarrier(ID3D12GraphicsCommandList2 *pCommandList, ID3D12Resource *pResource,
                         D3D12_RESOURCE_STATES stateBefore, D3D12_RESOURCE_STATES stateAfter);

    // For drawing specific meshes
    void DrawMesh(ID3D12GraphicsCommandList2 *pCommandList, const Model &mesh);
    void DrawMeshes(ID3D12GraphicsCommandList2 *pCommandList);
    void DrawInstancedMesh(ID3D12GraphicsCommandList2 *pCommandList, const Model &mesh, const UINT numInstances);

    void DrawQuad(ID3D12GraphicsCommandList2 *pCommandList);

    eastl::pair<XMMATRIX, XMMATRIX> GetLightSpaceMatrix(const float nearZ, const float farZ);
    // Doubt that't a good idea to return vector of matrices. Should rather pass vector as a parameter probalby and
    // fill it inside function.
    void GetLightSpaceMatrices(eastl::array<eastl::pair<XMMATRIX, XMMATRIX>, 4> &outMatrices);

    static eastl::array<XMVECTOR, 8> GetFrustumCornersWorldSpace(const XMMATRIX &view, const XMMATRIX &projection);

    D3D12_CPU_DESCRIPTOR_HANDLE GetRTV() const;
    D3D12_CPU_DESCRIPTOR_HANDLE GetDSV() const;

private:
    UINT m_dxgiFactoryFlags = 0u;

    UINT m_width;
    UINT m_height;
    float m_aspectRatio;

    HWND m_hWND;

    static inline bool m_bIsInitialized = false;
    bool m_bAreGraphicsFeaturesLoaded = false;
    bool m_bUseWarpDevice = false;

    bool m_appPaused = false;       // is the application paused ?
    bool m_minimized = false;       // is the application minimized ?
    bool m_maximized = false;       // is the application maximized ?
    bool m_resizing = false;        // are the resize bars being dragged ?
    bool m_fullscreenState = false; // fullscreen enabled
    bool m_isWireframe = false;     // Fill mode
    bool m_is4xMsaaState = false;

    bool m_enableDebugLayer = true;

    UINT m_4xMsaaQuality = 0u;

private:
    // Pipeline objects.
    eastl::shared_ptr<SwapChain> m_swapChain;

    ComPtr<ID3D12Resource> m_depthStencilBuffer;

    eastl::shared_ptr<RootSignature> m_rootSignature;
    eastl::shared_ptr<RootSignature> m_UUIDRootSignature;
    eastl::unordered_map<Shader::EShaderType, ComPtr<ID3DBlob>> m_shaders;
    eastl::unordered_map<PipelineStateObject::EPsoType, ComPtr<ID3D12PipelineState>> m_pipelineStates;

    eastl::unique_ptr<Blainn::DebugRenderer> m_debugRenderer;
    eastl::unique_ptr<Blainn::UIRenderer> m_UIRenderer;

    RenderTarget m_uuidRenderTarget;
    eastl::unique_ptr<SelectionManager> m_selectionManager = nullptr;

    PassConstants m_shadowPassCBData;
    PassConstants m_mainPassCBData;
    MaterialData m_perMaterialSBData;

    UINT m_pointLightsCount = 0u;
    UINT m_spotLightsCount = 0u;

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

    Camera *m_camera;
    eastl::shared_ptr<Camera> m_editorCamera;

#pragma region DeferredShading
    eastl::unique_ptr<GBuffer> m_GBuffer;
    UINT m_GBufferTexturesSrvHeapStartIndex = 0u;
#pragma endregion DeferredShading

#pragma region CascadedShadows
    UINT m_cascadesShadowSrvHeapStartIndex = 0u;
    eastl::unique_ptr<ShadowMap> m_cascadeShadowMap;
#pragma endregion CascadedShadows

#pragma region Textures
    UINT m_skyCubeSrvHeapStartIndex = 0u;
    UINT m_texturesSrvHeapStartIndex = 0u;
#pragma endregion Textures

    // TODO
    eastl::unique_ptr<struct MeshComponent> skyBox = nullptr;
    ComPtr<ID3D12Resource> skyBoxResource = nullptr;
    ComPtr<ID3D12Resource> skyBoxUploadHeap = nullptr;

private:
    // Probably not the best idea, but it is what it is ;=
    friend class UIRenderer;
};
} // namespace Blainn