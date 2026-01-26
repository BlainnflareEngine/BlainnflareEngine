#include "Common.hlsl"

struct PSInput
{
	float4 iPosH : SV_POSITION;
    
	nointerpolation uint iInstanceID : InstanceID;
};

float4 PointLightPS(PSInput input) : SV_TARGET
{
	float2 texCoord = input.iPosH.xy;
    
	PointLightInstanceData instData = gPointLights[input.iInstanceID];
    
    GBufferPixelData gBuffer = FetchGBufferData(input.iPosH);
	
    float4 diffuseAlbedo = gBuffer.diffuse;
    float4 ambientOcclusion = gBuffer.ao;
    float4 normalTex = gBuffer.normal;
	float4 specularTex = gBuffer.specular;
	
	float3 posW = ComputeWorldPos(float3(texCoord, 0.0f));
    
	float3 fresnelR0 = specularTex.xyz;
	float shininess = exp2(specularTex.a * 10.5f);
    
	Material mat = { diffuseAlbedo, fresnelR0, shininess };
    
	float3 N = normalize(normalTex.xyz);
	float3 toEye = gEyePos - posW;
	float3 viewDir = toEye / length(toEye);
    
    float3 pointLight = ComputePointLight(instData.Light, N, posW, viewDir, mat);
	return float4(pointLight, 1.0f);
}

float4 SpotLightPS(PSInput input) : SV_TARGET
{
    float2 texCoord = input.iPosH.xy;
    
    SpotLightInstanceData instData = gSpotLights[input.iInstanceID];
    
    GBufferPixelData gBuffer = FetchGBufferData(input.iPosH);
	
    float4 diffuseAlbedo = gBuffer.diffuse;
    float4 ambientOcclusion = gBuffer.ao;
    float4 normalTex = gBuffer.normal;
    float4 specularTex = gBuffer.specular;
	
    float3 posW = ComputeWorldPos(float3(texCoord, 0.0f));
    
    float3 fresnelR0 = specularTex.xyz;
    float shininess = exp2(specularTex.a * 10.5f);
    
    Material mat = { diffuseAlbedo, fresnelR0, shininess };
    
    float3 N = normalize(normalTex.xyz);
    float3 toEye = gEyePos - posW;
    float3 viewDir = toEye / length(toEye);
	
    float3 spotLight = ComputeSpotLight(instData.Light, N, posW, viewDir, mat);
    return float4(spotLight, 1.0f);
}