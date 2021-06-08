#include "PBRHelpers.hlsli"
#include "ShadowHelpers.hlsli"
#include "Helpers.hlsli"
#include "Structs.hlsli"
#include "Constants.hlsli"

#define THREAD_COUNT        16
#define TOTAL_THREAD_COUNT  (THREAD_COUNT * THREAD_COUNT)
#define MAX_LIGHTS_PER_TILE 1024

#ifdef DRAW_TILE_DEBUG
    #define DRAW_TILE_OCCUPANCY 1
#else
#define DRAW_TILE_OCCUPANCY 0
#endif

Texture2D<float4> AlbedoTexture : register(t0, space0);
Texture2D<float4> NormalTexture : register(t1, space0);
Texture2D<float4> MaterialTexture : register(t2, space0);
Texture2D<float4> DepthStencilTexture : register(t3, space0);
Texture2D<float4> DXRReflection : register(t4, space0);
TextureCube<float4> IrradianceMap : register(t5, space0);
TextureCube<float4> SpecularIrradianceMap : register(t6, space0);
Texture2D<float4> IntegrationLUT : register(t7, space0);
Texture2D<float> DirLightShadowMaps: register(t8, space0);
TextureCubeArray<float> PointLightShadowMaps : register(t9, space0);
Texture2D<float3> SSAO : register(t10, space0);

SamplerState LUTSampler : register(s0, space0);
SamplerState IrradianceSampler : register(s1, space0);

SamplerComparisonState ShadowMapSampler0 : register(s2, space0);
SamplerComparisonState ShadowMapSampler1 : register(s3, space0);

cbuffer Constants : register(b0, D3D12_SHADER_REGISTER_SPACE_32BIT_CONSTANTS) {
int NumPointLights;
int NumShadowCastingPointLights;
int NumSkyLightMips;
int ScreenWidth;
int ScreenHeight;
}

ConstantBuffer<Camera> CameraBuffer : register(b0, space0);

cbuffer PointLightsBuffer : register(b1, space) {
PointLight PointLights[MAX_LIGHTS_PER_TILE];
}

cbuffer PointLightsPosRadBuffer : register(b2, space0) {
PositionRadius PointLightsPosRad[MAX_LIGHTS_PER_TILE];
}

cbuffer ShadowCastingPointLightsBuffer : register(b3, space0) {
ShadowPointLight ShadowCastingPointLights[8];
}

cbuffer ShadowCastingPointLightsPosRadBuffer : register(b4, space0) {
PositionRadius ShadowCastingPointLightsPosRad[8];
}

ConstantBuffer<DirectionalLight> DirectionalLightBuffer : register(b5, space0);

RWTexture2D<float4> Output : register(u0, space0);

groupshared uint GroupMinZ;
groupshared uint GroupMaxZ;
groupshared uint GroupPointLightCounter;
groupshared uint GroupPointLightIndices[MAX_LIGHTS_PER_TILE];
groupshared uint GroupShadowPointLightCounter;
groupshared uint GroupShadowPointLightIndices[MAX_LIGHTS_PER_TILE];

float GetNumTilesX() {
	DivideByMultiple(ScreenWidth, THREAD_COUNT);
}

float GetNumTilesY() {
	DivideByMultiple(ScreenHeight, THREAD_COUNT);
}

[numthreads(THREAD_COUNT, THREAD_COUNT, 1)]
void Main(ComputeShaderInput input) {
	uint threadIndex = input.GroupThreadID.y * THREAD_COUNT + input.GroupThreadID.x;
	if (threadIndex == 0) {
		GroupMinZ = 0x7f7fffff;
		GroupMaxZ = 0;
	}

	GroupMemoryBarrierWithGroupSync();

	uint2 texCoord = input.DispatchThreadID.xy;
	float depth = DepthStencilTexture.Load(int3(texCoord, 0));
	float viewPosZ = Depth_ProjectionToView(depth, CameraBuffer.ProjectionInverse);

	uint z = asuint(viewPosZ);
	if (depth < 1.0f) {
		InterlockedMin(GroupMinZ, z);
		InterlockedMax(GroupMaxZ, z);
	}

	GroupMemoryBarrierWithGroupSync();

	float minZ = asfloat(GroupMinZ);
	float maxZ = asfloat(GroupMaxZ);

	float4 Frustum[4];
	{
		float pxm = float(THREAD_COUNT * input.GroupID.x);
		float pym = float(THREAD_COUNT * input.GroupID.y);
		float pxp = float(THREAD_COUNT * (input.GroupID.x + 1));
		float pyp = float(THREAD_COUNT * (input.GroupID.y + 1));
		float width = THREAD_COUNT * GetNumTilesX();
		float height = THREAD_COUNT * GetNumTilesY();

		float3 cornerPoints[4];
		cornerPoints[0] = Float3_ProjectionToView(
			float3((pxm / width) * 2.0f - 1.0f, ((height - pym) / height) * 2.0f - 1.0f, 1.0f),
			CameraBuffer.ProjectionInverse);

		cornerPoints[1] = Float3_ProjectionToView(
			float3((pxp / width) * 2.0f - 1.0f, ((height - pym) / height) * 2.0f - 1.0f, 1.0f),
			CameraBuffer.ProjectionInverse);

		cornerPoints[2] = Float3_ProjectionToView(
			float3((pxp / width) * 2.0f - 1.0f, ((height - pyp) / height) * 2.0f - 1.0f, 1.0f),
			CameraBuffer.ProjectionInverse);

		cornerPoints[3] = Float3_ProjectionToView(
			float3((pxm / width) * 2.0f - 1.0f, ((height - pyp) / height) * 2.0f - 1.0f, 1.0f),
			CameraBuffer.ProjectionInverse);

		for (uint i = 0; i < 4; i++) {
			Frustum[i] = CreatePlane(cornerPoints[i], cornerPoints[(i + 1) & 3]);
		}
	}

	if (threadIndex == 0) {
		GroupPointLightCounter = 0;
		GroupShadowPointLightCounter = 0;
	}

	GroupMemoryBarrierWithGroupSync();

	for (uint i = threadIndex; i < NumPointLights; i += TOTAL_THREAD_COUNT) {
		float3 pos = PointLightsPosRad[i].Position;
		float3 viewPos = mul(float4(pos, 1.0f), CameraBuffer.View).xyz;
		float radius = PointLightsPosRad[i].Radius;

		if (
			(GetSignedDistanceFromPlane(viewPos, Frustum[0]) < radius) &&
			(GetSignedDistanceFromPlane(viewPos, Frustum[1]) < radius) &&
			(GetSignedDistanceFromPlane(viewPos, Frustum[2]) < radius) &&
			(GetSignedDistanceFromPlane(viewPos, Frustum[3]) < radius) &&
			(-viewPos.z + minZ < radius) &&
			(viewPos.z - maxZ < radius)
		) {
			uint index = 0;
			InterlockedAdd(GroupPointLightCounter, 1, index);
			GroupPointLightIndices[index] = i;
		}
	}

	for (uint j = threadIndex; j < NumShadowCastingPointLights; j += TOTAL_THREAD_COUNT) {
		float3 pos = ShadowCastingPointLightsPosRad[j].Position;
		float3 viewPos = mul(float4(pos, 1.0f), CameraBuffer.View).xyz;
		float radius = ShadowCastingPointLightsPosRad[j].Radius;

		if (
			(GetSignedDistanceFromPlane(viewPos, Frustum[0]) < radius) &&
			(GetSignedDistanceFromPlane(viewPos, Frustum[1]) < radius) &&
			(GetSignedDistanceFromPlane(viewPos, Frustum[2]) < radius) &&
			(GetSignedDistanceFromPlane(viewPos, Frustum[3]) < radius) &&
			(-viewPos.z + minZ < radius) &&
			(viewPos.z - maxZ < radius)
		) {
			uint index = 0;
			InterlockedAdd(GroupPointLightCounter, 1, index);
			GroupPointLightIndices[index] = j;
		}
	}

	GroupMemoryBarrierWithGroupSync();

	const float2 texCoordFloat = float2(texCoord) / float2(ScreenWidth, ScreenHeight);
	const float3 worldPosition = PositionFromDepth(depth, texCoordFloat, CameraBuffer.ViewProjectionInverse);
	const float3 BufferAlbedo = AlbedoTexture.Load(int3(texCoord, 0)).rgb;
	const float3 BufferMaterial = MaterialTexture.Load(int3(texCoord, 0)).rgb;
	const float3 BufferNormal = NormalTexture.Load(int3(texCoord, 0)).rgb;
	const float ScreenSpaceAO = SSAO.Load(int3(texCoord, 0)).r;

	const float3 N = UnpackNormal(BufferNormal);
	const float3 V = normalize(CameraBuffer.Position - worldPosition);
	const float BufferRoughness = BufferMaterial.r;
	const float BufferMetallic = BufferMaterial.g;
	const float BufferAO = BufferMaterial.b * ScreenSpaceAO;

	float F0 = Float3(0.04f);
	F0 = lerp(F0, BufferAlbedo, BufferMetallic);

	float3 L0 = Float3(0.0f);

	for (uint i = 0; i < GroupPointLightCounter; i++) {
		int Index = GroupPointLightIndices[i];
		const PointLight light = PointLights[Index];
		const PositionRadius lightPosRad = PointLightsPosRad[Index];

		float3 L = lightPosRad.Position - worldPosition;
		float Distance = dot(L, L);
		float Attenuation = 1.0f / max(Distance, 0.01f * 0.01f);
		L = normalize(L);

		float3 incidentRadiance = light.Color * Attenuation;
		incidentRadiance = DirectRadiance(F0, N, V, L, incidentRadiance, BufferAlbedo, BufferRoughness, BufferMaterial);

		L0 += incidentRadiance;
	}

	for (uint i = 0; i < GroupShadowPointLightCounter; i++) {
		int Index = GroupPointLightIndices[i];
		const ShadowPointLight light = ShadowCastingPointLights[Index];
		const PositionRadius lightPosRad = ShadowCastingPointLightsPosRad[Index];

		float ShadowFactor = PointLightShadowFactor(PointLightShadowMaps, float(Index), ShadowMapSampler0,
		                                            worldPosition, N, light, lightPosRad);
		if (ShadowFactor > 0.001f) {
			float3 L = lightPosRad.Position - worldPosition;
			float distance = dot(L, L);
			float Attenuation = 1.0f / max(distance, 0.01f * 0.01f);
			L = normalize(L);

			float3 incidentRadiance = light.Color * Attenuation;
			incidentRadiance = DirectRadiance(F0, N, V, L, incidentRadiance, BufferAlbedo, BufferRoughness,
			                                  BufferMetallic);
			L0 += incidentRadiance * ShadowFactor;
		}
	}

	{
		const DirectionalLight light = DirectionalLightBuffer;
		const float ShadowFactor = DirectionalLightShadowFactor(DirLightShadowMaps, ShadowMapSampler1, worldPosition, N,
		                                                        light);
		if (ShadowFactor > 0.001f) {
			float3 L = normalize(-light.Direction);

			float3 incidentRadiance = light.Color;
			incidentRadiance = DirectRadiance(F0, N, V, L, incidentRadiance, BufferAlbedo, BufferRoughness,
			                                  BufferMetallic);

			L0 += incidentRadiance * ShadowFactor;
		}
	}

	float3 FinalColor = L0;
	{
		const float NDotV = max(dot(N, V), 0.0f);

		float3 F = FresnelSchlickRoughness(F0, V, N, BufferRoughness);
		float3 Ks = F;
		float3 Kd = Float3(1.0f) - Ks;
		float3 irradiance = IrradianceMap.SampleLevel(IrradianceSampler, N, 0.0f).rgb;
		float3 diffuse = irradiance * BufferAlbedo * Kd;

		float3 R = reflect(-V, N);
		float3 prefilteredMap = SpecularIrradianceMap.SampleLevel(IrradianceSampler, R,
		                                                          BufferRoughness * (NumSkyLightMips - 1.0f)).rgb;
		float2 BRDFIntegration = IntegrationLUT.SampleLevel(LUTSampler, float2(NDotV, BufferRoughness), 0.0f).rg;
		float3 Specular = prefilteredMap * (F * BRDFIntegration.x + BRDFIntegration.y);

		float3 Ambient = (diffuse + Specular) * BufferAO;
		FinalColor = Ambient + L0;
	}

#if DRAW_TILE_OCCUPANCY
    const uint TotalLightCount = GroupPointLightCounter + GroupShadowPointLightCounter;

	float4 tint = Float4(1.0f);
	if (TotalLightCount < 8) {
		float col = float(TotalLightCount) / 8.0f;
		tint = float4(0.0f, col, 0.0f, 1.0f);
	} else if (TotalLightCount < 16) {
		float col = float(TotalLightCount) / 16.0f;
		tint = float4(0.0f, col, col, 1.0f);
	} else if (TotalLightCount < 32) {
		float col = float(TotalLightCount) / 32.0f;
		tint = float4(0.0f, 0.0f, col, 1.0f);
	} else if (TotalLightCount < 64) {
		float col = float(TotalLightCount) / 64.0f;
		tint = float4(col, col, 0.0f, 1.0f);
	} else {
		float col = float(TotalLightCount) / float(NumPointLights + NumShadowCastingPointLights);
		tint = float4(col, 0.0f, 0.0f, 1.0f);
	}

	FinalColor = FinalColor * tint.rgb;
	
#endif

	float FinalLuminance = Luminance(FinalColor);
	FinalColor = ApplyGammaCorrectionAndTonemapping(FinalColor);
	Output[texCoord] = float4(FinalColor, FinalLuminance);
}
