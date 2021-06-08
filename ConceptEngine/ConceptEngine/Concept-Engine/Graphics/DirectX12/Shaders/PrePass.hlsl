#include "Structs.hlsli"
#include "Constants.hlsli"

cbuffer TransformBuffer : register(b0, D3D12_SHADER_REGISTER_SPACE_32BIT_CONSTANTS) {
float4x4 TransformMat;
}

ConstantBuffer<Camera> CameraBuffer : register(b0, space0);

struct VSInput {
	float3 Position : POSITION0;
	float3 Normal : NORMAL0;
	float3 Tangent : TANGENT0;
	float2 TexCoord : TEXCOORD0;
};

float4 Main(VSInput input) : SV_POSITION {
	float4 worldPosition = mul(float4(input.Position, 1.0f), TransformMat);
	return mul(worldPosition, CameraBuffer.ViewProjection);
}
