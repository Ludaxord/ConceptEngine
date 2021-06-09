#include "PBRHelpers.hlsli"
#include "Structs.hlsli"
#include "RayTracingHelpers.hlsli"

RaytracingAccelerationStructure Scene : register(t0, space0);

TextureCube<float4> Skybox : register(t1, space0);
Texture2D<float4> BufferNormal : register(t2, space0);
Texture2D<float4> BufferDepth : register(t3, space0);

ConstantBuffer<Camera> CameraBuffer : register(b0, space0);

SamplerState BufferSampler : register(s0, space0);

RWTexture2D<float4> OutTexture : register(u0, space0);

[shader("raygeneration")]
void RayGen() {
	uint3 dispatchIndex = DispatchRaysIndex();
	uint3 dispatchDimensions = DispatchRaysDimensions();

	float2 texCoord = float2(dispatchIndex.xy) / float2(dispatchDimensions.xy);

	float depth = BufferDepth.SampleLevel(BufferSampler, texCoord, 0).r;

	float3 worldPosition = PositionFromDepth(depth, texCoord, CameraBuffer.ViewProjectionInverse);
	float3 worldNormal = BufferNormal.SampleLevel(BufferSampler, texCoord, 0).rgb;
	worldNormal = UnpackNormal(worldNormal);

	float3 viewDir = normalize(worldPosition - CameraBuffer.Position);

	uint3 launchIndex = DispatchRaysIndex();
	uint3 launchDim = DispatchRaysDimensions();

	float2 crd = float2(launchIndex.xy);
	float2 dims = float2(launchDim.xy);

	float2 d = ((crd / dims) * 2.0f - 1.0f);

	float3 forward = normalize(CameraBuffer.Forward);
	float3 right = normalize(cross(float3(0.0f, 1.0f, 0.0f), forward));
	float3 up = normalize(-cross(right, forward));

	float3 direction = forward + (right * (d.x * CameraBuffer.AspectRatio)) + (up * (-d.y));

	RayDesc desc;
	desc.Origin = CameraBuffer.Position;
	desc.Direction = normalize(direction);
	desc.TMin = CameraBuffer.NearPlane;
	desc.TMax = 10000.0f;

	RayPayload payload;
	payload.CurrentDepth = 1;

	TraceRay(Scene, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, 0xff, 0, 0, 0, desc, payload);

	OutTexture[dispatchIndex.xy] = float4(ApplyGammaCorrectionAndTonemapping(payload.Color), 1.0f);
}
