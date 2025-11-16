#include "Common.hlsl"

struct PSInput
{
    float4 iPosH : SV_POSITION;
};

float4 main(PSInput input) : SV_TARGET
{
    // look at the DeferredPointLightPS.hlsl, they are almost identical (in terms of deferred shading logic)
    return 1.0f;
}