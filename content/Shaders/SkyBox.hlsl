//=============================================================================
// Sky.fx by Frank Luna (C) 2011 All Rights Reserved.
//=============================================================================

#include "Common.hlsl"

struct VSInput
{
    float3 iPosL : POSITION;
};

struct VSOutput
{
    float4 oPosH : SV_POSITION;
    float3 oPosL : POSITION;
};
 
VSOutput VSMain(VSInput input)
{
    VSOutput output = (VSOutput) 0;

	// Use local vertex position as cubemap lookup vector.
    output.oPosL = input.iPosL;
	
	// Transform to world space.
    float4 posW = mul(float4(input.iPosL, 1.0f), gWorld);

	// Always center sky about camera.
    posW.xyz += gEyePos;

	// Set z = w so that z/w = 1 (i.e., skydome always on far plane).
    output.oPosH = mul(posW, gViewProj).xyww;
	
    return output;
}

float4 PSMain(VSOutput input) : SV_Target
{
    return gCubeMap.Sample(gSamplerAnisotropicWrap, input.oPosL);
}