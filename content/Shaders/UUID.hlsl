cbuffer cbPerObject : register(b0)
{
    float4x4 gWorld;
    uint4 idData;
    //uint idData[4];
};

cbuffer cbPerPass : register(b1)
{
    float4x4 gViewProj;
};

struct VSInput
{
    float3 iPosL : POSITION0;
    float3 iNormalL : NORMAL;
    float3 iTangentU : TANGENT;
    float3 iBitangentU : BITANGENT;
    float2 iTexC : TEXCOORD0;
};

float4 VSMain(VSInput input) : SV_POSITION
{
    float4 posW = mul(float4(input.iPosL, 1.0), gWorld);
    float4 posH = mul(posW, gViewProj);
    return posH;
}

uint4 PSMain(float4 pos : SV_POSITION) : SV_Target
{
    //return uint4(idData[0], idData[1], idData[2], idData[3]);
    return idData;
}

