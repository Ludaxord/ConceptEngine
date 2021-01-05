struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

cbuffer ConstantBuffer : register(b0)
{
    float4 colorMultiplier;
};

PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
{
    PSInput result;

    result.position = position;
    result.color = color * colorMultiplier;
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}
