#include "PBRHelpers.hlsli"
#include "Structs.hlsli"
#include "RayTracingHelpers.hlsli"
#include "Constants.hlsli"

RaytracingAccelerationStructure Scene : register(t0, space0);

ConstantBuffer<Camera> CameraBuffer : register(b0, space0);

TextureCube<float4> Skybox : register(t1, space0);
Texture2D<float4> MaterialTextures[128] : register(t4, space0);

SamplerState TextureSampler : register(s1, space0);

StructuredBuffer<Vertex> Vertices : register(t0, D3D12_SHADER_REGISTER_SPACE_RT_LOCAL);
ByteAddressBuffer Indices : register(t1, D3D12_SHADER_REGISTER_SPACE_RT_LOCAL);

[shader("closesthit")]
void ClosestHit(inout RayPayload payload, in BuiltInTriangleIntersectionAttributes intersectionAttributes) {
	payload.Color = float3(1.0f, 0.0f, 0.0f);
	payload.CurrentDepth = payload.CurrentDepth + 1;

	const uint indexSizeInBytes = 4;
	const uint indicesPerTriangle = 3;
	const uint triangleIndexStride = indicesPerTriangle * indexSizeInBytes;
	const uint baseIndex = PrimitiveIndex() * triangleIndexStride;

	uint3 indices = Indices.Load3(baseIndex);

	float3 triangleNormals[3] = {
		Vertices[indices[0]].Normal,
		Vertices[indices[1]].Normal,
		Vertices[indices[2]].Normal
	};

	float3 barycentricCoords = float3(
		1.0f - intersectionAttributes.barycentrics.x - intersectionAttributes.barycentrics.y,
		intersectionAttributes.barycentrics.x,
		intersectionAttributes.barycentrics.y
	);

	float3 normal = (triangleNormals[0] * barycentricCoords.x) + (triangleNormals[1] * barycentricCoords.y) + (
		triangleNormals[2] * barycentricCoords.z);
	normal = normalize(normal);

	float3 triangleTangent[3] = {
		Vertices[indices[0]].Tangent,
		Vertices[indices[1]].Tangent,
		Vertices[indices[2]].Tangent
	};

	float2 triangleTexCoords[3] = {
		Vertices[indices[0]].TexCoord,
		Vertices[indices[1]].TexCoord,
		Vertices[indices[2]].TexCoord
	};

	float2 texCoords = (triangleTexCoords[0] * barycentricCoords.x) + (triangleTexCoords[1] * barycentricCoords.y) + (
		triangleTexCoords[2] * barycentricCoords.z);
	texCoords.y = 1.0f - texCoords.y;

	float3 tangent = (triangleTangent[0] * barycentricCoords.x) + (triangleTangent[1] * barycentricCoords.y) + (
		triangleTangent[2] * barycentricCoords.z);
	tangent = normalize(tangent);

	uint textureIndex = InstanceID();
	uint albedoIndex = textureIndex;
	uint normalIndex = textureIndex + 1;

	float3 mappedNormal = MaterialTextures[normalIndex].SampleLevel(TextureSampler, texCoords, 0).rgb;
	mappedNormal = UnpackNormal(mappedNormal);

	float3 bitangent = normalize(cross(normal, tangent));
	normal = ApplyNormalMapping(mappedNormal, normal, tangent, bitangent);

	float LOD = (min(RayTCurrent(), 1000.0f) / 1000.0f) * 15.0f;
	float3 AlbedoColor = ApplyGamma(MaterialTextures[albedoIndex].SampleLevel(TextureSampler, texCoords, LOD).rgb);

	const float3 HitPosition = WorldHitPosition();
	const float3 LightDirection = normalize(float3(0.0f, 1.0f, 0.0f));
	const float3 ViewDirection = normalize(CameraBuffer.Position - HitPosition);

	const float SampledAO = 1.0f;
	const float SampledMetallic = 1.0f;
	const float SampledRoughness = 1.0f;
	const float finalRoughness = min(max(SampledRoughness, MIN_ROUGHNESS), MAX_ROUGHNESS);

	float3 F0 = Float3(0.04f);
	F0 = lerp(F0, AlbedoColor, SampledMetallic);

	float3 IncidentRadiance = Float3(10.0f);
	float3 L0 = DirectRadiance(F0,
	                          normal,
	                          ViewDirection,
	                          LightDirection,
	                          IncidentRadiance,
	                          AlbedoColor,
	                          SampledRoughness,
	                          SampledMetallic);

	float3 ambient = Float3(0.03f) * AlbedoColor * SampledAO;
	float3 color = ambient + L0;

	payload.Color = color;
}
