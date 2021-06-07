#include "PBRHelpers.hlsli"
#include "Structs.hlsli"
#include "RayTracingHelpers.hlsli"
#include "Constants.hlsli"

RaytracingAccelerationStructure Scene : register(t0, space0);

ConstantBuffer<Camera> CameraBuffer : register(b0, space0);

TextureCube<float4> Skybox : register(t1, space0);
Texture2D<float4> MaterialTextures[128] : register(t4, space0);

StructuredBuffer<Vertex> Vertices : register(t0, D3D12_SHADER_REGISTER_SPACE_RT_LOCAL);
ByteAddressBuffer Indices : register(t1, D3D12_SHADER_REGISTER_SPACE_RT_LOCAL);

[shader("closestHit")]
void ClosestHit() {

}
