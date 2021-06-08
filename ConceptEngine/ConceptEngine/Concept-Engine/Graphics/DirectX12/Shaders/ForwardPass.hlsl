#include "PBRHelpers.hlsli"
#include "Helpers.hlsli"
#include "Structs.hlsli"
#include "ShadowHelpers.hlsli"

#if ENABLE_PARALLAX_MAPPING
#define PARALLAX_MAPPING_ENABLED
#endif

#if ENABLE_NORMAL_MAPPING
#define NORMAL_MAPPING_ENABLED
#endif

ConstantBuffer<Camera> CameraBuffer : register(b0, space0);

cbuffer PointLightsBuffer : register(b1, space0) {
PointLight PointLights[32];
}

cbuffer PointLightsPosRadBuffer : register(b2, space0) {
PositionRadius PointLightsPosRad[32];
}

cbuffer ShadowCastingPointLightsBuffer : register(b3, space0) {
ShadowPointLight ShadowCastingPointLights[8];
}

cbuffer ShadowCastingPointLightsPosRadBuffer : register(b4, space0) {
PositionRadius ShadowCastingPointLightsPosRad[8];
}

ConstantBuffer<DirectionalLight> DirLightBuffer : register(b5, space0);

ConstantBuffer<Transform> TransformBuffer : register(b0, D3D12_SHADER_REGISTER_SPACE_32BIT_CONSTANTS);
ConstantBuffer<Material> MaterialBuffer : register(b6, space0);

SamplerState MaterialSampler : register(s0, space0);
SamplerState LUTSampler : register(s1, space0);
SamplerState IrradianceSampler : register(s2, space0);

SamplerComparisonState ShadowMapSampler0 : register(s3, space0);
SamplerComparisonState ShadowMapSampler1 : register(s4, space0);

TextureCube<float4> IrradianceMap : register(t0, space0);
TextureCube<float4> SpecularIrradianceMap : register(t1, space0);
Texture2D<float4> IntegrationLUT : register(t2, space0);
Texture2D<float> DirLightShadowMaps : register(t3, space0);
TextureCubeArray<float> PointLightShadowMaps : register(t4, space0);

Texture2D<float4> AlbedoTexture : register(t5, space0);
#ifdef NORMAL_MAPPING_ENABLED
Texture2D<float4> NormalTexture : register(t6, space0);
#endif
Texture2D<float> RoughnessTexture : register(t7, space0);
#ifdef PARALLAX_MAPPING_ENABLED
Texture2D<float> HeightMap : register(t8, space0);
#endif
Texture2D<float> MetallicTexture : register(t9, space0);
Texture2D<float> AOTexture : register(t10, space0);
Texture2D<float> AlphaTexture : register(t11, space0);

struct VSInput {
	float3 Position : POSITION0;
	float3 Normal : NORMAL0;
	float3 Tangent : TANGENT0;
	float2 TexCoord : TEXCOORD0;
};


struct VSOutput {
	float3 WorldPosition : POSITION0;
	float3 Normal : NORMAL0;
#ifdef NORMAL_MAPPING_ENABLED
	float3 Tangent : TANGENT0;
	float3 Bitangent : BITANGENT0;
#endif
	float2 TexCoord : TEXCOORD0;
#ifdef PARALLAX_MAPPING_ENABLED
	float3 TangentViewPos : TANGENTVIEWPOS0;
	float3 TangentPosition : TANGENTPOSITION0;
#endif
	float4 Position : SV_Position;
};

struct PSInput {
	float3 WorldPosition : POSITION0;
	float3 Normal : NORMAL0;
#ifdef NORMAL_MAPPING_ENABLED
	float3 Tangent : TANGENT0;
	float3 Bitangent : BITANGENT0;
#endif
	float2 TexCoord : TEXCOORD0;
#ifdef PARALLAX_MAPPING_ENABLED
    float3 TangentViewPos  : TANGENTVIEWPOS0;
    float3 TangentPosition : TANGENTPOSITION0;
#endif
	bool IsFrontFace : SV_IsFrontFace;
};

VSOutput VSMain(VSInput input) {
	VSOutput vsOutput;

	float3 normal = normalize(mul(float4(input.Normal, 0.0f), TransformBuffer.Transform).xyz);
	vsOutput.Normal = normal;

#ifdef NORMAL_MAPPING_ENABLED
	float3 tangent = normalize(mul(float4(input.Tangent, 0.0f), TransformBuffer.Transform).xyz);
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	vsOutput.Tangent = tangent;

	float3 bitangent = normalize(cross(vsOutput.Tangent, vsOutput.Normal));
	vsOutput.Bitangent = bitangent;
#endif

	vsOutput.TexCoord = input.TexCoord;

	float4 worldPosition = mul(float4(input.Position, 1.0f), TransformBuffer.Transform);
	vsOutput.Position = mul(worldPosition, CameraBuffer.ViewProjection);
	vsOutput.WorldPosition = worldPosition.xyz;

#ifdef PARALLAX_MAPPING_ENABLED
	float3x3 TBN = float3x3(tangent, bitangent, normal);
	TBN = transpose(TBN);

	vsOutput.TangentViewPos = mul(CameraBuffer.Position, TBN);
	vsOutput.TangentPosition = mul(worldPosition.xyz, TBN);
#endif

	return vsOutput;
}

#ifdef PARALLAX_MAPPING_ENABLED

static const float HEIGHT_SCALE = 0.03f;

float SampleHeightMap(float2 TexCoords) {
	return 1.0f - HeightMap.Sample(MaterialSampler, TexCoords).r;
}

float2 ParallaxMapping(float2 texCoords, float3 viewDir) {
	const float minLayers = 32;
	const float maxLayers = 64;

	float numLayers = lerp(maxLayers, minLayers, abs(dot(float3(0.0f, 0.0f, 1.0f), viewDir)));
	float layerDepth = 1.0f / numLayers;

	float2 P = viewDir.xy / viewDir.z * HEIGHT_SCALE;
	float2 deltaTexCoords = P / numLayers;

	float2 currentTexCoords = texCoords;
	float currentDepthMapValue = SampleHeightMap(currentTexCoords);

	float currentLayerDepth = 0.0f;
	while (currentLayerDepth < currentDepthMapValue) {
		currentTexCoords -= deltaTexCoords;
		currentDepthMapValue = SampleHeightMap(currentTexCoords);
		currentLayerDepth += layerDepth;
	}

	float2 prevTexCoords = currentTexCoords + deltaTexCoords;

	float afterDepth = currentDepthMapValue - currentLayerDepth;
	float beforeDepth = SampleHeightMap(prevTexCoords) - currentLayerDepth + layerDepth;

	float weight = afterDepth / (afterDepth - beforeDepth);
	float2 finalTexCoords = prevTexCoords * weight + currentTexCoords * (1.0f - weight);

	return finalTexCoords;
}

#endif

float4 PSMain(PSInput Input) : SV_Target0 {
	float2 texCoords = Input.TexCoord;
	texCoords.y = 1.0f - texCoords.y;

#ifdef PARALLAX_MAPPING_ENABLED
	if (MaterialBuffer.EnableWeight != 0) {
		float3 viewDir = normalize(Input.TangentViewPos - Input.TangentPosition);
		texCoords = ParallaxMapping(texCoords, viewDir);
		if (texCoords.x > 1.0f || texCoords.y > 1.0f || texCoords.x < 0.0f || texCoords.y < 0.0f) {
			discard;
		}
	}
#endif

	float3 SampledAlbedo = ApplyGamma(AlbedoTexture.Sample(MaterialSampler, texCoords).rgb) * MaterialBuffer.Albedo;

	const float3 WorldPosition = Input.WorldPosition;
	const float V = normalize(CameraBuffer.Position - WorldPosition);
	float3 N = normalize(Input.Normal);
	if (!Input.IsFrontFace) {
		N = -N;
	}

#ifdef NORMAL_MAPPING_ENABLED
	float3 SampledNormal = NormalTexture.Sample(MaterialSampler, texCoords);
	SampledNormal = UnpackNormal(SampledNormal);

	float3 tangent = normalize(Input.Tangent);
	float3 bitangent = normalize(Input.Bitangent);
	float3 normal = normalize(N);
	N = ApplyNormalMapping(SampledNormal, normal, tangent, bitangent);
#endif

	const float SampledAO = AOTexture.Sample(MaterialSampler, texCoords) * MaterialBuffer.AO;
	const float SampledMetallic = MetallicTexture.Sample(MaterialSampler, texCoords) * MaterialBuffer.Metallic;
	const float SampledRoughness = RoughnessTexture.Sample(MaterialSampler, texCoords) * MaterialBuffer.Roughness;
	const float SampledAlpha = AlphaTexture.Sample(MaterialSampler, texCoords);
	const float Roughness = SampledRoughness;
	if (SampledAlpha < 0.1f) {
		discard;
	}

	float3 F0 = Float3(0.04f);
	F0 = lerp(F0, SampledAlbedo, SampledMetallic);

	float NDotV = max(dot(N, V), 0.0f);
	float3 L0 = Float3(0.0f);

	//TODO: Add passing to point lights
	for (int i = 0; i < 0; i++) {
		const PointLight light = PointLights[i];
		const PositionRadius LightPosRad = PointLightsPosRad[i];

		float3 L = LightPosRad.Position - WorldPosition;
		float distance = dot(L, L);
		float Attenuation = 1.0f / max(distance, 0.01f * 0.01f);
		L = normalize(L);

		float3 incidentRadiance = light.Color * Attenuation;
		incidentRadiance = DirectRadiance(F0, N, V, L, incidentRadiance, SampledAlbedo, Roughness, SampledMetallic);

		L0 += incidentRadiance;
	}

	for (int i = 0; i < 4; i++) {
		const ShadowPointLight light = ShadowCastingPointLights[i];
		const PositionRadius lightPosRad = ShadowCastingPointLightsPosRad[i];

		float ShadowFactor = PointLightShadowFactor(PointLightShadowMaps, float(i), ShadowMapSampler0, WorldPosition, N,
		                                            light, lightPosRad);
		if (ShadowFactor > 0.001f) {
			float3 L = lightPosRad.Position - WorldPosition;
			float distance = dot(L, L);
			float Attenuation = 1.0f / max(distance, 0.01f * 0.01f);
			L = normalize(L);

			float3 incidentRadiance = light.Color * Attenuation;
			incidentRadiance = DirectRadiance(F0, N, V, L, incidentRadiance, SampledAlbedo, Roughness, SampledMetallic);

			L0 += incidentRadiance * ShadowFactor;
		}
	}

	{
		const DirectionalLight light = DirLightBuffer;
		const float shadowFactor = DirectionalLightShadowFactor(DirLightShadowMaps, ShadowMapSampler1, WorldPosition, N,
		                                                        light);
		if (shadowFactor > 0.001f) {
			float3 L = normalize(-light.Direction);
			float3 H = normalize(L + V);

			float3 incidentRadiance = light.Color;
			incidentRadiance = DirectRadiance(F0, N, V, L, incidentRadiance, SampledAlbedo, Roughness, SampledMetallic);

			L0 += incidentRadiance * shadowFactor;
		}
	}

	float3 FinalColor = L0;
	{
		const float NDotV = max(dot(N, V), 0.0f);

		float3 F = FresnelSchlickRoughness(F0, V, N, Roughness);
		float3 Ks = F;
		float3 Kd = Float3(1.0f) - Ks;
		float3 irradiance = IrradianceMap.SampleLevel(IrradianceSampler, N, 0.0f).rgb;
		float3 diffuse = irradiance * SampledAlbedo * Kd;

		float3 R = reflect(-V, N);
		float3 prefilteredMap = SpecularIrradianceMap.SampleLevel(IrradianceSampler, R, Roughness * (7.0f - 1.0f)).rgb;
		float2 BRDFIntegration = IntegrationLUT.SampleLevel(LUTSampler, float2(NDotV, Roughness), 0.0f).rg;
		float3 specular = prefilteredMap * (F * BRDFIntegration.x + BRDFIntegration.y);

		float3 ambient = (diffuse + specular) * SampledAO;
		FinalColor = ambient + L0;
	}

	float FinalLuminance = Luminance(FinalColor);
	FinalColor = ApplyGammaCorrectionAndTonemapping(FinalColor);
	return float4(FinalColor, FinalLuminance);

}
