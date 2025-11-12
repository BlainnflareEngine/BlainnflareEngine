#pragma once

#include <Windows.h>
#include "Render/FrameResource.h"
#include "Render/DXHelpers.h"

namespace Blainn
{
    class Renderer;
    class Device;

    class RenderSubsystem
    {
    private:
        RenderSubsystem() = default; 
        RenderSubsystem(const RenderSubsystem&) = delete;
        RenderSubsystem& operator=(const RenderSubsystem&) = delete; 
        RenderSubsystem(const RenderSubsystem&&) = delete;
        RenderSubsystem& operator=(const RenderSubsystem&&) = delete; 
    public:
        static RenderSubsystem &GetInstance();
        void Init(HWND windowHandle);
        void Render(float deltaTime);
        void Destroy();
        
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
            Opaque = 0,
            WireframeOpaque,
            Transparency,
            CascadedShadowsOpaque,

            DeferredGeometry,
            DeferredDirectional,

            DeferredPointWithinFrustum,
            DeferredPointIntersectsFarPlane,
            DeferredPointFullQuad,
            
            DeferredSpot,
            
            NumPipelineStates = 10u
        };

        enum EShaderType : UINT
        {
            DefaultVS = 0,
            DefaultOpaquePS,
            CascadedShadowsVS,
            CascadedShadowsGS,

            DeferredGeometryVS,
            DeferredGeometryPS,
            DeferredDirVS,
            DeferredDirPS,
            DeferredLightVolumesVS,
            DeferredPointPS,
            DeferredSpotPS,

            NumShaders = 11U
        };

    public:
        // Engine(UINT width, UINT height, const std::wstring& name, const std::wstring& className);

        virtual void OnUpdate(float deltaTime);
        virtual void OnRender(float deltaTime);
        virtual void OnDestroy();

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
        // void DrawInstancedRenderItems(ID3D12GraphicsCommandList* cmdList, std::vector<std::unique_ptr<RenderItem>>& renderItems);
        void DrawQuad(ID3D12GraphicsCommandList* cmdList);

    private:
        std::vector<std::unique_ptr<FrameResource>> m_frameResources;
        FrameResource* m_currFrameResource = nullptr;
        int m_currFrameResourceIndex = 0;

        UINT m_passCbvOffset = 0;

        float m_sunPhi = XM_PIDIV4;
        float m_sunTheta = 1.25f * XM_PI;
        
        ComPtr<ID3D12RootSignature> m_rootSignature;

        ComPtr<ID3D12DescriptorHeap> m_cbvHeap; // Heap for constant buffer views
        ComPtr<ID3D12DescriptorHeap> m_srvHeap; // Heap for textures
    
        std::unordered_map<EShaderType, ComPtr<ID3DBlob>> m_shaders;
        std::unordered_map<EPsoType, ComPtr<ID3D12PipelineState>> m_pipelineStates;

        ObjectConstants m_perObjectCBData;
        PassConstants m_shadowPassCBData;
        PassConstants m_geometryPassCBData;
        PassConstants m_mainPassCBData; // deferred color(light) pass
        //PassConstants m_lightingPassCBData;
        MaterialData m_perMaterialSBData;
        // InstanceData m_perInstanceSBData;

        // std::unordered_map<std::string, std::unique_ptr<MeshGeometry>> m_geometries;
        // std::unordered_map<std::string, std::unique_ptr<Material>> m_materials;
        // std::unordered_map<std::string, std::unique_ptr<Texture>> m_textures;

        // std::vector<std::unique_ptr<RenderItem>> m_renderItems;
        // std::vector<std::unique_ptr<RenderItem>> m_pointLights;
        // std::vector<RenderItem*> m_opaqueItems;

        // std::unique_ptr<Camera> m_camera;
        // std::unique_ptr<ShadowMap> m_cascadeShadowMap;

        // std::unique_ptr<MeshGeometry> m_fullQuad;

    #pragma region DeferredRendering
        // std::unique_ptr<GBuffer> m_GBuffer;

        CD3DX12_GPU_DESCRIPTOR_HANDLE m_cascadeShadowSrv;
        CD3DX12_GPU_DESCRIPTOR_HANDLE m_GBufferTexturesSrv;
        float m_shadowCascadeLevels[MaxCascades] = { 0.0f, 0.0f, 0.0f, 0.0f };

    private:
        VOID LoadPipeline();
        VOID Reset();
        VOID CreateRtvAndDsvDescriptorHeaps();
        
        VOID LoadAssets();
        VOID CreateRootSignature();
        VOID CreateShaders();
        VOID CreatePSO();
        
        VOID LoadScene();
        VOID LoadTextures();
        // Shapes
        VOID CreateGeometry();
        // Propertirs of shapes' surfaces to model light interaction
        VOID CreateGeometryMaterials();
        // Shapes could constist of some items to render
        VOID CreateSceneObjects();
        VOID CreateRenderItems();
        VOID CreatePointLights();
        VOID CreateFrameResources();
        // Heaps are created if there are root descriptor tables in root signature 
        VOID CreateDescriptorHeaps();

        VOID PopulateCommandList();
        VOID MoveToNextFrame();

        std::array<const CD3DX12_STATIC_SAMPLER_DESC, 5> GetStaticSamplers();

        std::pair<XMMATRIX, XMMATRIX> GetLightSpaceMatrix(const float nearPlane, const float farPlane);
        // Doubt that't a good idea to return vector of matrices. Should rather pass vector as a parameter probalby and fill it inside function.
        void GetLightSpaceMatrices(std::vector<std::pair<XMMATRIX, XMMATRIX>>& outMatrices);
        void CreateShadowCascadeSplits();

        std::vector<XMVECTOR> GetFrustumCornersWorldSpace(const XMMATRIX& view, const XMMATRIX& projection);
    };
} // namespace Blainn