#pragma once

using namespace DirectX;

namespace Blainn
{
	enum class EPassType : uint8_t
	{
		// ComputePass
		// ZPrePass
		DepthShadow = 0,
		DeferredGeometry,
		DeferredLighting,
		ForwardLighting, // could be helpful for particles, skybox and other forward-like shit
		NumPasses = 4u
	};

	/*
	 * Graphics features
	 */

	/*
	 * Shadows
	 * MACROS MaxCascades has to correspond its define value in shaders
	 */

	#define MaxCascades (4)

	struct CascadesShadows
	{
		CascadesShadows()
		{
			for (int i = 0; i < MaxCascades; ++i)
			{
				CascadeViewProj[i] = XMMatrixIdentity();
				Distances[i] = 0.0f;
			}
		}

		XMMATRIX CascadeViewProj[MaxCascades];
		float Distances[MaxCascades];
	};

	struct DirectionalLightData
	{
        XMFLOAT4 Color = {1.0f, 1.0f, 0.9f, 1.0f}; // (Color * TempRGB), Intensity
		XMFLOAT3 Direction = { 0.5f, -1.0f, 0.5f };
        float pad;
	};

	struct PointLightData
    {
        XMFLOAT4 Color = {1.0f, 1.0f, 0.9f, 1.0f}; // (Color * TempRGB), Intensity
        XMFLOAT3 Position = {0.0f, 0.0f, 0.0f};
		float FalloffStart = 0.1f;
        XMFLOAT3 TempRGB = {0.0f, 0.0f, 0.0f}; // padding actually 
        float FalloffEnd = 1.0f;
    };

	struct SpotLightData
    {
        XMFLOAT4 Color = {1.0f, 1.0f, 0.9f, 1.0f}; // (Color * TempRGB), Intensity (SpotPower)
        XMFLOAT3 Direction = {0.5f, -1.0f, 0.5f};
        float FalloffStart = 0.1f;
        XMFLOAT3 Position = {0.0f, 0.0f, 0.0f};
        float FalloffEnd = 1.0f;
    };

	/*
	 * Constant buffers types
	 */

	struct ObjectConstants
	{
		XMFLOAT4X4 World;
		XMFLOAT4X4 InvTransposeWorld;
		XMFLOAT4X4 TexTransform;
		uint32_t MaterialIndex = 0u;
		uint32_t objPad0 = 0u;
		uint32_t objPad1 = 0u;
		uint32_t objPad2 = 0u;
	};

	struct PointLightInstanceData
	{
		XMFLOAT4X4 World;
        PointLightData Light;
	};

	struct SpotLightInstanceData
    {
        XMFLOAT4X4 World;
        SpotLightData Light;
    };

	struct PassConstants
	{
		XMFLOAT4X4 View;
		XMFLOAT4X4 Proj;
		XMFLOAT4X4 ViewProj;
		XMFLOAT4X4 InvViewProj;

		CascadesShadows Cascades;

		XMFLOAT3 EyePosW = { 0.0f, 0.0f, 0.0f };
		float pad1 = 0.0f;
		
		XMFLOAT2 RenderTargetSize = { 0.0f, 0.0f };
		XMFLOAT2 InvRenderTargetSize = { 0.0f, 0.0f };

		float NearZ = 0.0f;
		float FarZ = 0.0f;
		float DeltaTime = 0.0f;
		float TotalTime = 0.0f;

		XMFLOAT4 Ambient = { 0.0f, 0.0f, 0.0f, 1.0f };

		/*XMFLOAT4 FogColor = { 0.7f, 0.7f, 0.7f, 1.0f };
		float FogStart = 8.0f;
		float FogRange = 18.0f;*/

		DirectionalLightData DirLight;
	};

	/*
	 * Structured buffers types
	 */
	struct MaterialData
	{
		XMFLOAT4 DiffuseAlbedo = { 1.0f, 1.0f, 1.0f, 1.0f };
		XMFLOAT3 FresnelR0 = { 0.01f, 0.01f, 0.01f };
		float Roughness = 0.25f;
		XMFLOAT4X4 MatTransform;
		uint32_t DiffuseMapIndex = 0u;
        uint32_t NormalMapIndex = 0u;
        uint32_t RoughnessMapIndex = 0u;
        uint32_t MetallicMapIndex = 0u;
	};
}