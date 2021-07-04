#include "Constants.hlsli"

cbuffer TransformBuffer : register(b0, D3D12_SHADER_REGISTER_SPACE_32BIT_CONSTANTS) {
float4x4 Transform;
float ShadowOffset;
}

cbuffer LightBuffer : register(b0, space0) {
float4x4 LightProjection;
float3 LightPosition;
float LightFarPlane;
}

struct VSInput {
	float3 Position : POSITION0;
	float3 Normal : NORMAL0;
	float3 Tangent : TANGENT0;
	float2 TexCoord : TEXCOORD0;
};

struct VSOutput {
	float3 WorldPosition : POSITION0;
	float4 Position : SV_POSITION;
};

float4 Main(VSInput input) : SV_POSITION {
	float3 Normal = normalize(input.Normal);
	float3 position = input.Position + (Normal * ShadowOffset);

	float4 worldPosition = mul(float4(position, 1.0f), Transform);
	return mul(worldPosition, LightProjection);
}

VSOutput VSMain(VSInput input) {
	VSOutput output = (VSOutput)0;

	float4 worldPosition = mul(float4(input.Position, 1.0f), Transform);
	output.WorldPosition = worldPosition.xyz;
	output.Position = mul(worldPosition, LightProjection);

	return output;
}

float PSMain(float3 worldPosition : POSITION0) : SV_Depth {
	float lightDistance = length(worldPosition.xyz - LightPosition);
	lightDistance = lightDistance / LightFarPlane;
	return lightDistance;
}

float4 VSM_VSMain(VSInput input) : SV_Position {
	float4 worldPosition = mul(float4(input.Position, 1.0f), Transform);
	return mul(worldPosition, LightProjection);
}

float4 VSM_PSMain(float4 Position : SV_Position) : SV_Tangent0 {
	float depth = Position.z;
	return float4(depth, depth * depth, 0.0f, 1.0f);
}
