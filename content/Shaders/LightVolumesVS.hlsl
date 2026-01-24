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

VSOutput main(VSInput input, uint instanceID : SV_InstanceID)
{
    VSOutput output = (VSOutput) 0;
    
    PointLightInstanceData instData = gPointLights[instanceID];
    
    float4 posW = mul(float4(input.iPosL, 1.0f), instData.World);
    output.oPosH = mul(posW, gViewProj);
    output.oInstanceID = instanceID;
    return output;
}