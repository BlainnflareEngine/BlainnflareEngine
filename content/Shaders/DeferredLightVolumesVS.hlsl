#include "Common.hlsl"

struct VSInput
{
    float3 iPosL : POSITION0;
};

struct VSOutput
{
    float4 oPosH : SV_POSITION;
    // for instancing (not sure that's the best way to implement this logic)
    nointerpolation uint oInstanceID : InstanceID;
};

VSOutput PointLightVS(VSInput input, uint instanceID : SV_InstanceID)
{
    VSOutput output = (VSOutput) 0;
    
    PointLightInstanceData instData = gPointLights[instanceID];
    
    // Uniform scale multiply
    instData.World._11 *= instData.Light.FalloffEnd;
    instData.World._22 *= instData.Light.FalloffEnd;
    instData.World._33 *= instData.Light.FalloffEnd;
    
    float4 posW = mul(float4(input.iPosL, 1.0f), instData.World);
    output.oPosH = mul(posW, gViewProj);
    output.oInstanceID = instanceID;
    return output;
}

VSOutput SpotLightVS(VSInput input, uint instanceID : SV_InstanceID)
{
    VSOutput output = (VSOutput) 0;
    
    SpotLightInstanceData instData = gSpotLights[instanceID];
    float4 posW = mul(float4(input.iPosL, 1.0f), instData.World);
    output.oPosH = mul(posW, gViewProj);
    output.oInstanceID = instanceID;
    return output;
}