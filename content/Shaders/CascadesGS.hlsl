
// the geometry shader is invoked per primitive

#include "Common.hlsl"

struct GSInput
{
    float4 iPosH : POSITION0;
};

struct GSOutput
{
    float4 oPosH : SV_POSITION;
    uint arrInd : SV_RenderTargetArrayIndex;
};

[instance(MaxCascades)]
//the maximum number of vertices the geometry shader will output for a single invocation
[maxvertexcount(3)]
void main(triangle GSInput p[3], in uint id : SV_GSInstanceID, inout TriangleStream<GSOutput> stream)
{
    [unroll]
    for (int i = 0; i < 3; ++i)
    {
        GSOutput output = (GSOutput) 0;
        
        output.oPosH = mul(float4(p[i].iPosH.xyz, 1.0f), gCascadeData.CascadeViewProj[id]);
        
        output.arrInd = id;
        stream.Append(output);
    }
}