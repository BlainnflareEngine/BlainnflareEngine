#include "Common.hlsl"

struct PSInput
{
    float4 iPosH : SV_POSITION;
    float3 iPosW : POSITION0;
    float3 iNormalW : NORMAL;
    float3 iTangentW : TANGENT;
    float3 iBitangentW : BITANGENT;
    float2 iTexC : TEXCOORD0;
};

struct GBuffer
{
    float4 DiffuseAlbedo : SV_Target0;
    float4 AmbientOcclusion : SV_Target1;
    float4 Normal : SV_Target2; // could be used only 2 components instead of 4
    float4 Specular : SV_Target3;
};

[earlydepthstencil]
GBuffer main(PSInput input)
{
    GBuffer output = (GBuffer) 0;
    
    MaterialData matData = gMaterialData[gMaterialIndex];
    
    // Interpolating normal can unnormalize it, so renormalize it.
    input.iNormalW = normalize(input.iNormalW);
    
    float4 diffuseAlbedo = matData.DiffuseAlbedo;
    float3 fresnelR0 = matData.FresnelR0;
    float roughness = matData.Roughness;
    
    uint diffuseTexIndex = matData.DiffuseMapIndex;
    uint normalTexIndex = matData.NormalMapIndex;
    uint metallicTexIndex = matData.MetallicMapIndex;
    uint roughnessTexIndex = matData.RoughnessMapIndex;
    
    if (diffuseTexIndex != INVALID_INDEX)
    {   
        diffuseAlbedo *= gTextures[diffuseTexIndex].Sample(gSamplerAnisotropicWrap, input.iTexC);
    }
    
    output.DiffuseAlbedo = diffuseAlbedo;
    output.AmbientOcclusion = float4(input.iPosW, 0.0f); // temporary
    output.Specular = float4(fresnelR0, log2(1.0f - roughness) / 10.5f);
    
    output.Normal = float4(input.iNormalW, 1.0f);
    
    if (normalTexIndex != INVALID_INDEX)
    {
        float4 normalMapSample = gTextures[normalTexIndex].Sample(gSamplerAnisotropicWrap, input.iTexC);
        output.Normal.xyz = NormalSampleToWorldSpace(normalMapSample.rgb, input.iNormalW, input.iTangentW);
        output.Normal.a = normalMapSample.a;
    }
    
    //if (metallicTexIndex != INVALID_INDEX)
    //{
    //    float4 metallicMapSample = gTextures[metallicTexIndex].Sample(gSamplerAnisotropicWrap, input.iTexC);
    //    if(metallicMapSample.r != 0.0f)
    //        output.DiffuseAlbedo = float4(0.0f, 1.0f, 0.0f, 0.0f);
    //}
    
    //if (roughnessTexIndex != INVALID_INDEX)
    //{
    //    float4 roughnessMapSample = gTextures[384 + roughnessTexIndex].Sample(gSamplerAnisotropicWrap, input.iTexC);
    //}
    
    return output;
}