#include "Common.hlsl"

struct VSInput
{
    float3 iPosL : POSITION0;
    float3 iNormalL : NORMAL;
    float3 iTangent : TANGENT;
    float2 inTexC : TEXCOORD0;
};

struct VSOutput
{
    float4 oPosH : SV_POSITION;
    float3 oPosW : POSITION0;
    float3 oNormalW : NORMAL;
    float2 oTexC : TEXCOORD0;
};

VSOutput main(VSInput input)
{
    VSOutput output = (VSOutput) 0;
    
    MaterialData matData = gMaterialData[gMaterialIndex];
    
    float4 oPosW = mul(float4(input.iPosL, 1.0f), gWorld);
    output.oPosH = mul(oPosW, gViewProj);
    output.oPosW = oPosW.xyz;
    output.oNormalW = mul(input.iNormalL, (float3x3) gInvTransposeWorld);
    
    float4 texCoord = mul(float4(input.inTexC, 0.0f, 1.0f), gTexTransform);
    output.oTexC = mul(texCoord, matData.MatTransform).xy;
    
    return output;
}