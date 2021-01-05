struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

// cbuffer ConstantBuffer : register(b0)
// {
//     float4 colorMultiplier;
// };

cbuffer ConstantBuffer : register(b0)
{
    float4x4 wvpMat;
};

PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
{
    PSInput result;

    result.position = mul(position, wvpMat);
    // result.position = position;
    // result.color = color * colorMultiplier;
    result.color = color;
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}
