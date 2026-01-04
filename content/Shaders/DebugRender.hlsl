cbuffer cbPerPass : register(b0)
{
    float4x4 gViewProj;
};

cbuffer cbPerObject : register(b1)
{
    float4 color;
};

struct VSIn
{
    float3 position : POSITION;
};

struct PSIn
{
    float4 position : SV_POSITION;
};

PSIn DebugVS(VSIn input)
{
    PSIn output;
    output.position = mul(float4(input.position, 1.0f), gViewProj);
    return output;
}

float4 DebugPS(PSIn input) : SV_TARGET
{
    return color;
    //return float4(1.0f, 0.0f, 1.0f, 1.0f);
}
