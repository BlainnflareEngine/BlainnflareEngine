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
    uint gNormalMapIndex; // todo
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
    
    uint NumDirLights = 0u;
    uint NumPointLights = 0u;
    
    Light gDirLight;
};

StructuredBuffer<InstanceData /*Light*/> gPointLights : register(t0, space1);
StructuredBuffer<InstanceData /*Light*/> gSpotLights : register(t1, space1);
Texture2D gGBuffer[GBufferSize] : register(t2, space1); // t2, t3, t4, t5, t6 in space1

Texture2DArray gShadowMaps : register(t0);
StructuredBuffer<MaterialData> gMaterialData : register(t1);
Texture2D gDiffuseMap[6 /*magic hardcode*/] : register(t2); // t2, t3, t4, t5, t6, t7 in space0

SamplerState gSamplerPointWrap : register(s0);
SamplerState gSamplerLinearWrap : register(s1);
SamplerState gSamplerAnisotropicWrap : register(s2);
SamplerState gShadowSamplerLinearBorder : register(s3);
SamplerComparisonState gShadowSamplerComparisonLinearBorder : register(s4);


float3 ComputeWorldPos(float3 texcoord)
{
    float depth = gGBuffer[G_DEPTH].Load(int3(texcoord)).r;

    float2 uv = texcoord.xy / gRTSize;
    float4 ndc = float4(uv.x * 2.0f - 1.0f, 1.0f - 2.0f * uv.y, depth, 1.0f);
    float4 worldPos = mul(ndc, gInvViewProj);
    worldPos.xyz /= worldPos.w;
    return worldPos.xyz;
}