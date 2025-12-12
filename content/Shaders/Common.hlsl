#define MaxCascades 4

// Deferred Rendering
#define GBufferSize 5 // should be sync with GBuffer class

#ifndef DIFFUSE_ALBEDO
#define G_DIFF_ALBEDO 0
#endif
#ifndef AMBIENT_OCCLUSION
#define G_AMB_OCCL 1
#endif
#ifndef NORMAL
#define G_NORMAL 2
#endif
#ifndef SPECULAR
#define G_SPECULAR 3
#endif
#ifndef DEPTH
#define G_DEPTH 4
#endif

#include "LightUtil.hlsl"

struct CascadesShadows
{
    float4x4 CascadeViewProj[MaxCascades];
    float4 Distances;
};

cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
    float4x4 gInvTransposeWorld;
    float4x4 gTexTransform;
    uint gMaterialIndex;
    uint gObjPad0;
    uint gObjPad1;
    uint gObjPad2;
};

struct InstanceData
{
    float4x4 gWorld;
    Light gLight;
};

cbuffer cbPerPass : register(b1)
{
    float4x4 gView;
    float4x4 gProj;
    float4x4 gViewProj;
    float4x4 gInvViewProj;
    CascadesShadows gCascadeData;
    float3 gEyePos;
    float gPassPad0;
    float2 gRTSize;
    float2 gInvRTSize;
    float gNearZ;
    float gFarZ;
    float gDeltaTime;
    float gTotalTime;
    
    float4 gAmbient;
    
    float4 gFogColor;
    float gFogStart;
    float gFogRange;
    
    Light gDirLight;
};

StructuredBuffer<MaterialData> gMaterialData : register(t0);
StructuredBuffer<InstanceData /*Light*/> gPointLights : register(t1);
StructuredBuffer<InstanceData /*Light*/> gSpotLights : register(t2);

Texture2DArray gShadowMaps : register(t0, space1);
Texture2D gGBuffer[GBufferSize] : register(t1, space1); // t1, t2, t3, t4, t5 in space1
TextureCube gCubeMap : register(t6, space1);
Texture2D gDiffuseMap[]: register(t7, space1);

SamplerState gSamplerPointWrap : register(s0);
SamplerState gSamplerLinearWrap : register(s1);
SamplerState gSamplerAnisotropicWrap : register(s2);
SamplerState gShadowSamplerLinearBorder : register(s3);
SamplerComparisonState gShadowSamplerComparisonLinearBorder : register(s4);

// Add in specular reflections (very approximated model)
float3 ComputeSpecularReflections(float3 toEyeW, float3 normalW, Material mat)
{
    float3 r = reflect(-toEyeW, normalW);
    float4 reflectionColor = gCubeMap.Sample(gSamplerAnisotropicWrap, r);
    float3 fresnelFactor = SchlickApproximation(mat.FresnelR0, normalW, r);
    return (mat.Shininess * fresnelFactor * reflectionColor.rgb);
}

float3 ComputeWorldPos(float3 texcoord)
{
    float depth = gGBuffer[G_DEPTH].Load(int3(texcoord)).r;

    float2 uv = texcoord.xy / gRTSize;
    float4 ndc = float4(uv.x * 2.0f - 1.0f, 1.0f - 2.0f * uv.y, depth, 1.0f);
    float4 worldPos = mul(ndc, gInvViewProj);
    worldPos.xyz /= worldPos.w;
    return worldPos.xyz;
}

float3 NormalSampleToWorldSpace(float3 normalMapSample, float3 unitNormalW, float3 tangentW)
{
	// Uncompress each component from [0,1] to [-1,1].
    float3 normalT = 2.0f * normalMapSample - 1.0f;

	// Build orthonormal basis.
    float3 N = unitNormalW;
    float3 T = normalize(tangentW - dot(tangentW, N) * N);
    float3 B = cross(N, T);

    float3x3 TBN = float3x3(T, B, N);

	// Transform from tangent space to world space.
    float3 bumpedNormalW = mul(normalT, TBN);

    return bumpedNormalW;
}