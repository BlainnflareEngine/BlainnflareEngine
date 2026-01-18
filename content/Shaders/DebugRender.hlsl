cbuffer cbPerPass : register(b0)
{
    float4x4 gViewProj;
};

struct VSIn
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PSIn
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSIn DebugVS(VSIn input)
{
    PSIn output;
    output.position = mul(float4(input.position, 1.0f), gViewProj);
    output.color = input.color;
    return output;
}

float4 DebugPS(PSIn input) : SV_TARGET
{
    return input.color;
    //return float4(1.0f, 0.0f, 1.0f, 1.0f);
}
