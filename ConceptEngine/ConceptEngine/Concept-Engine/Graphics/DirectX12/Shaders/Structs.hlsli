#ifndef STRUCTS_HLSLI
#define STRUCTS_HLSLI

struct ComputeShaderInput {
	uint3 GroupID : SV_GroupID;
	uint3 GroupThreadID : SV_GroupThreadID;
	uint3 DispatchThreadID : SV_DispatchThreadID;
	uint GroupIndex : SV_GroupIndex;
};

struct Camera {
	float4x4 ViewProjection;
	float4x4 View;
	float4x4 ViewInverse;
	float4x4 Projection;
	float4x4 ProjectionInverse;
	float4x4 ViewProjectionInverse;
	float3 Position;
	float NearPlane;
	float3 Forward;
	float FarPlane;
	float AspectRatio;
};

struct Vertex {
	float3 Position;
	float3 Normal;
	float3 Tangent;
	float2 TexCoord;
};

#endif
