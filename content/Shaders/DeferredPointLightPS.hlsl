#include "Common.hlsl"

struct PSInput
{
	float4 iPosH : SV_POSITION;
    
	//nointerpolation uint iInstanceID : InstanceID;
};

float4 main(PSInput input) : SV_TARGET
{
	float2 texCoord = input.iPosH.xy;
    
	//InstanceData instData = gPointLights[input.iInstanceID];
    
	float4 diffuseAlbedo = gGBuffer[G_DIFF_ALBEDO].Load(input.iPosH.xyz);
	float4 ambientOcclusion = gGBuffer[G_AMB_OCCL].Load(input.iPosH.xyz);
	float4 normalTex = gGBuffer[G_NORMAL].Load(input.iPosH.xyz);
	float4 specularTex = gGBuffer[G_SPECULAR].Load(input.iPosH.xyz);
	float3 posW = ComputeWorldPos(float3(texCoord, 0.0f));
    
	float3 fresnelR0 = specularTex.xyz;
	float shininess = exp2(specularTex.a * 10.5f);
    
	Material mat = { diffuseAlbedo, fresnelR0, shininess };
    
	float3 N = normalize(normalTex.xyz);
	float3 toEye = gEyePos - posW;
	float3 viewDir = toEye / length(toEye);
    
	//float3 pointLight = CalcPointLight(instData.gLight, N, posW, viewDir, mat);
	//return float4(pointLight, 1.0f);
    return 1.0f.xxxx;
}

//struct PSInput
//{
//    float4 iPosH : SV_POSITION;
    
//    nointerpolation uint iInstanceID : InstanceID;
//};

//float4 main(PSInput input) : SV_TARGET
//{
//    float2 texCoord = input.iPosH.xy;
    
//    InstanceData instData = gPointLights[input.iInstanceID];
    
//    float4 diffuseAlbedo = gGBuffer[G_DIFF_ALBEDO].Load(input.iPosH.xyz);
//    float4 ambientOcclusion = gGBuffer[G_AMB_OCCL].Load(input.iPosH.xyz);
//    float4 normalTex = gGBuffer[G_NORMAL].Load(input.iPosH.xyz);
//    float4 specularTex = gGBuffer[G_SPECULAR].Load(input.iPosH.xyz);
//    float2 motionVectorsTex = gGBuffer[G_MOTION_VEC].Load(input.iPosH.xyz).xy;
//    float3 posW = ComputeWorldPos(float3(texCoord, 0.0f));
    
//    float3 fresnelR0 = specularTex.xyz;
//    const float shininess = exp2(specularTex.a * 10.5f) * normalTex.a;
    
//    Material mat = { diffuseAlbedo, fresnelR0, shininess };
    
//    float3 toEye = gEyePos - posW;
//    float3 viewDir = toEye / length(toEye);
    
//    float3 pointLight = CalcPointLight(instData.gLight, normalTex.xyz, posW, viewDir, mat);
//    // Does not work properly
//    //pointLight += ComputeSpecularReflections(toEye, N, mat);
    
//    return float4(pointLight, 1.0f);
//}