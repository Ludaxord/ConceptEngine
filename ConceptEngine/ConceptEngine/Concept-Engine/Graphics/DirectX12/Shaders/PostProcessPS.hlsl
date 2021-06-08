Texture2D FinalImage : register(t0, space0);
SamplerState PointSampler : register(s0, space0);

float4 Main(float2 texCoord : TEXCOORD0) : SV_TARGET {
	float3 color = FinalImage.Sample(PointSampler, texCoord).rgb;
	return float4(color, 1.0f);
}
