#include "Helpers.hlsli"
#include "Constants.hlsli"

cbuffer CameraBuffer : register(b0, D3D12_SHADER_REGISTER_SPACE_32BIT_CONSTANTS) {
float4x4 ViewProjection;
}

TextureCube<float4> Skybox : register(t0, space0);
SamplerState SkyboxSampler : register(s0, space0);

struct VSInput {
	float3 Position : POSITION0;
	float3 Normal : NORMAL0;
	float3 Tangent : TANGENT0;
	float2 TexCoord : TEXCOORD0;
};

struct VSOutput {
	float3 TexCoord : TEXCOORD0;
	float4 Position : SV_POSITION0;
};

VSOutput VSMain(VSInput input) {
	VSOutput vsOutput;
	vsOutput.TexCoord = input.Position;
	vsOutput.Position = mul(float4(input.Position, 1.0f), ViewProjection);
	vsOutput.Position = vsOutput.Position.xyww;
	return vsOutput;
}

float4 PSMain(float3 texCoord : TEXCOORD0) : SV_TARGET0 {
	float3 color = Skybox.Sample(SkyboxSampler, texCoord).rgb;
	float finalLuminance = Luminance(color);
	color = ApplyGammaCorrectionAndTonemapping(color);
	return float4(color, finalLuminance);
}
