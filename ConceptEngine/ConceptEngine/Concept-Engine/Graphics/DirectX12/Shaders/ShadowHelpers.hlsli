#ifndef SHADOW_HELPERS_HLSLI
#define SHADOW_HELPERS_HLSLI

#include "Structs.hlsli"
#include "Helpers.hlsli"

#define POINT_LIGHT_SAMPLES 4
#define OFFSET_SAMPLES 20

static const float3 SampleOffsetDirections[OFFSET_SAMPLES] = {
	float3(1.0f, 1.0f, 1.0f), float3(1.0f, -1.0f, 1.0f), float3(-1.0f, -1.0f, 1.0f), float3(-1.0f, 1.0f, 1.0f),
	float3(1.0f, 1.0f, -1.0f), float3(1.0f, -1.0f, -1.0f), float3(-1.0f, -1.0f, -1.0f), float3(-1.0f, 1.0f, -1.0f),
	float3(1.0f, 1.0f, 0.0f), float3(1.0f, -1.0f, 0.0f), float3(-1.0f, -1.0f, 0.0f), float3(-1.0f, 1.0f, 0.0f),
	float3(1.0f, 0.0f, 1.0f), float3(-1.0f, 0.0f, 1.0f), float3(1.0f, 0.0f, -1.0f), float3(-1.0f, 0.0f, -1.0f),
	float3(0.0f, 1.0f, 1.0f), float3(0.0f, -1.0f, 1.0f), float3(0.0f, -1.0f, -1.0f), float3(0.0f, 1.0f, -1.0f)
};

float PointLightShadowFactor(
	in TextureCube<float> ShadowMap,
	in SamplerComparisonState Sampler,
	float3 WorldPosition,
	float3 Normal,
	ShadowPointLight light,
	PositionRadius lightPos
) {
	const float3 directionToLight = WorldPosition - lightPos.Position;
	const float3 lightDirection = normalize(lightPos.Position - WorldPosition);

	const float ShadowBias = max(light.MaxShadowBias * (1.0f - (max(dot(Normal, lightDirection), 0.0f))),
	                             light.ShadowBias);
	float depth = length(directionToLight) / light.FarPlane;

	depth = (depth - ShadowBias);

	float shadow = 0.0f;
	const float diskRadius = (0.4f + (depth)) / light.FarPlane;

	[unroll]
	for (int i = 0; i < POINT_LIGHT_SAMPLES; i++) {
		const int Index = int(float(OFFSET_SAMPLES) * Random(floor(WorldPosition.xyz * 1000.0f), i)) % OFFSET_SAMPLES;
		shadow += ShadowMap.SampleCmpLevelZero(Sampler, directionToLight + SampleOffsetDirections[Index] * diskRadius,
		                                       depth);
	}

	shadow = shadow / POINT_LIGHT_SAMPLES;
	return min(shadow, 1.0f);
}

float PointLightShadowFactor(
	in TextureCubeArray<float> ShadowMap,
	float Index,
	in SamplerComparisonState Sampler,
	float3 WorldPosition,
	float3 Normal,
	ShadowPointLight light,
	PositionRadius lightPos
) {
	const float3 directionToLight = WorldPosition - lightPos.Position;
	const float3 lightDirection = normalize(lightPos.Position - WorldPosition);

	const float ShadowBias = max(light.MaxShadowBias * (1.0f - (max(dot(Normal, lightDirection), 0.0f))),
	                             light.ShadowBias);
	float depth = length(directionToLight) / light.FarPlane;

	depth = (depth - ShadowBias);

	float shadow = 0.0f;
	const float diskRadius = (0.4f + (depth)) / light.FarPlane;

	[unroll]
	for (int i = 0; i < POINT_LIGHT_SAMPLES; i++) {
		const int OffsetIndex = int(float(OFFSET_SAMPLES) * Random(floor(WorldPosition.xyz * 1000.0f), i)) %
			OFFSET_SAMPLES;

		const float3 sampleVec = directionToLight + SampleOffsetDirections[OffsetIndex] * diskRadius;
		shadow += ShadowMap.SampleCmpLevelZero(Sampler, float4(sampleVec, Index), depth);
	}

	shadow = shadow / POINT_LIGHT_SAMPLES;
	return min(shadow, 1.0f);
}

//Just for test
float PointLightShadowFactor(
	in TextureCubeArray<float> ShadowMap,
	float Index,
	SamplerComparisonState Sampler,
	float3 WorldPosition,
	float3 Normal,
	ShadowPointLight light,
	PositionRadius lightPos
) {
	const float3 directionToLight = WorldPosition - lightPos.Position;
	const float3 lightDirection = normalize(lightPos.Position - WorldPosition);

	const float ShadowBias = max(light.MaxShadowBias * (1.0f - (max(dot(Normal, lightDirection), 0.0f))),
	                             light.ShadowBias);
	float depth = length(directionToLight) / light.FarPlane;

	depth = (depth - ShadowBias);

	float shadow = 0.0f;
	const float diskRadius = (0.4f + (depth)) / light.FarPlane;

	[unroll]
	for (int i = 0; i < POINT_LIGHT_SAMPLES; i++) {
		const int OffsetIndex = int(float(OFFSET_SAMPLES) * Random(floor(WorldPosition.xyz * 1000.0f), i)) %
			OFFSET_SAMPLES;

		const float3 sampleVec = directionToLight + SampleOffsetDirections[OffsetIndex] * diskRadius;
		shadow += ShadowMap.SampleCmpLevelZero(Sampler, float4(sampleVec, Index), depth);
	}

	shadow = shadow / POINT_LIGHT_SAMPLES;
	return min(shadow, 1.0f);
}

#define PCF_RANGE 2
#define PCF_WIDTH float((PCF_RANGE * 2) + 1)

#define ENABLE_POISSON_FILTERING 0
#define ENABLE_VSM 0

#if ENABLE_POISSON_FILTERING

#define POISSON_SAMPLES 2
#define TOTAL_POISSON_SAMPLES 16

static const float2 PoissonDisk[16] = {
	float2(-0.94201624, -0.39906216),
	float2(0.94558609, -0.76890725),
	float2(-0.094184101, -0.92938870),
	float2(0.34495938, 0.29387760),
	float2(-0.91588581, 0.45771432),
	float2(-0.81544232, -0.87912464),
	float2(-0.38277543, 0.27676845),
	float2(0.97484398, 0.75648379),
	float2(0.44323325, -0.97511554),
	float2(0.53742981, -0.47373420),
	float2(-0.26496911, -0.41893023),
	float2(0.79197514, 0.19090188),
	float2(-0.24188840, 0.99706507),
	float2(-0.81409955, 0.91437590),
	float2(0.19984126, 0.78641367),
	float2(0.14383161, -0.14100790)
};

float CalculatePoissonShadow(float3 WorldPosition, float2 TexCoord, float CompareDepth, float FarPlane) {
	float shadow = 0.0f;
	const float diskRadius = (0.4f + (CompareDepth)) / FarPlane;

	[unroll]
	for (int i = 0; i < POISSON_SAMPLES; i++) {
		int Index = int() % TOTAL_POISSON_SAMPLES;
		shadow += DirLightShadowMaps.SampleCmpLevelZero(ShadowMapSampler0,
		                                                TexCoord.xy + (PoissonDisk[Index] * diskRadius), CompareDepth);
	}

	shadow = shadow / POISSON_SAMPLES;
	return min(shadow, 1.0f);
}

#elif ENABLE_VSM

float CalculateVSM(float2 TexCoords, float CompareDepth) {
	float2 moments = (float2)0;

	[unroll]
	for (int x = -PCF_RANGE; x <= PCF_RANGE; x++) {
		[unroll]
		for (int y = -PCF_RANGE; y <= PCF_RANGE; y++) {
			moments += DirLightShadowMaps.Sample(ShadowMapSampler1, TexCoords, int2(x, y)).rg;
		}
	}

	moments = moments / (PCF_WIDTH * PCF_WIDTH);

	float variance = max(moments.y - moments.x * moments.x, MIN_VALUE);
	float P = moments.x - CompareDepth;
	float Md_2 = P * P;
	float pMax = Linstep(0.2f, 1.0f, variance / (variance + Md_2));
	return min(max(P, pMax), 1.0f);
}

#else

float StandardShadow(
	in Texture2D<float> ShadowMap,
	in SamplerComparisonState Sampler,
	float2 texCoords,
	float compareDepth
) {
	float shadow = 0.0f;
	[unroll]
	for (int x = -PCF_RANGE; x <= PCF_RANGE; x++) {
		[unroll]
		for (int y = -PCF_RANGE; y <= PCF_RANGE; y++) {
			shadow += ShadowMap.SampleCmpLevelZero(Sampler, texCoords, compareDepth, int2(x, y)).r;
		}
	}

	shadow = shadow / (PCF_WIDTH * PCF_WIDTH);
	return min(shadow, 1.0f);
}

//Just for test
float StandardShadow(
	in Texture2D<float> ShadowMap,
	SamplerComparisonState Sampler,
	float2 texCoords,
	float compareDepth
) {
	float shadow = 0.0f;
	[unroll]
	for (int x = -PCF_RANGE; x <= PCF_RANGE; x++) {
		[unroll]
		for (int y = -PCF_RANGE; y <= PCF_RANGE; y++) {
			shadow += ShadowMap.SampleCmpLevelZero(Sampler, texCoords, compareDepth, int2(x, y)).r;
		}
	}

	shadow = shadow / (PCF_WIDTH * PCF_WIDTH);
	return min(shadow, 1.0f);
}

#endif

float DirectionalLightShadowFactor(
	in Texture2D<float> ShadowMap,
	in SamplerComparisonState Sampler,
	float3 WorldPosition,
	float3 N,
	DirectionalLight Light
) {
	float4 LightSpacePosition = mul(float4(WorldPosition, 1.0f), Light.LightMatrix);
	float3 L = normalize(-Light.Direction);

	float3 projCoords = LightSpacePosition.xyz / LightSpacePosition.w;
	projCoords.xy = (projCoords.xy * 0.5f) + 0.5f;
	projCoords.y = 1.0f - projCoords.y;

	float depth = projCoords.z;
	if (depth >= 1.0f) {
		return 1.0f;
	}

	float shadowBias = max(Light.MaxShadowBias * (1.0f - (max(dot(N, L), 0.0f))), Light.ShadowBias);
	float biasedDepth = (depth - shadowBias);
    return StandardShadow(ShadowMap, Sampler, projCoords.xy, biasedDepth);
}

//just for tests
float DirectionalLightShadowFactor(
	in Texture2D<float> ShadowMap,
	SamplerComparisonState Sampler,
	float3 WorldPosition,
	float3 N,
	DirectionalLight Light
) {
	float4 LightSpacePosition = mul(float4(WorldPosition, 1.0f), Light.LightMatrix);
	float3 L = normalize(-Light.Direction);

	float3 projCoords = LightSpacePosition.xyz / LightSpacePosition.w;
	projCoords.xy = (projCoords.xy * 0.5f) + 0.5f;
	projCoords.y = 1.0f - projCoords.y;

	float depth = projCoords.z;
	if (depth >= 1.0f) {
		return 1.0f;
	}

	float shadowBias = max(Light.MaxShadowBias * (1.0f - (max(dot(N, L), 0.0f))), Light.ShadowBias);
	float biasedDepth = (depth - shadowBias);
    return StandardShadow(ShadowMap, Sampler, projCoords.xy, biasedDepth);
}

#endif
