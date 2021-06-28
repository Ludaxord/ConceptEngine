#include "PBRHelpers.hlsli"
#include "Structs.hlsli"
#include "Constants.hlsli"

#if ENABLE_PARALLAX_MAPPING
#define PARALLAX_MAPPING_ENABLED
#endif

#if ENABLE_NORMAL_MAPPING
#define NORMAL_MAPPING_ENABLED
#endif

ConstantBuffer<Camera> CameraBuffer : register(b0, space0);

SamplerState MaterialSampler : register(s0, space0);

ConstantBuffer<Transform> TransformBuffer : register(b0, D3D12_SHADER_REGISTER_SPACE_32BIT_CONSTANTS);
ConstantBuffer<Material> MaterialBuffer : register(b1, space0);

Texture2D<float4> AlbedoMap : register(t0, space0);
#ifdef NORMAL_MAPPING_ENABLED
Texture2D<float4> NormalMap : register(t1, space0);
#endif
Texture2D<float4> RoughnessMap : register(t2, space0);
#ifdef PARALLAX_MAPPING_ENABLED
Texture2D<float4> HeightMap : register(t3, space0);
#endif
Texture2D<float4> MetallicMap : register(t4, space0);
Texture2D<float4> AOMap : register(t5, space0);

struct VSInput {
	float3 Position : POSITION0;
	float3 Normal : NORMAL0;
	float3 Tangent : TANGENT0;
	float2 TexCoord : TEXCOORD0;
};

struct VSOutput {
	float3 Normal : NORMAL0;
	float3 ViewNormal : NORMAL1;
#if defined(NORMAL_MAPPING_ENABLED) || defined(PARALLAX_MAPPING_ENABLED)
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
	float3 Normal : NORMAL0;
	float3 ViewNormal : NORMAL1;
#if defined(NORMAL_MAPPING_ENABLED) || defined(PARALLAX_MAPPING_ENABLED)
	float3 Tangent : TANGENT0;
	float3 Bitangent : BITANGENT0;
#endif
	float2 TexCoord : TEXCOORD0;
#ifdef PARALLAX_MAPPING_ENABLED
	float3 TangentViewPos : TANGENTVIEWPOS0;
	float3 TangentPosition : TANGENTPOSITION0;
#endif
};

struct PSOutput {
	float4 Albedo : SV_Target0;
	float4 Normal : SV_Target1;
	float4 Material : SV_Target2;
	float4 ViewNormal : SV_Target3;
};

#ifdef PARALLAX_MAPPING_ENABLED
static const float HEIGHT_SCALE = 0.03f;

float SampleHeightMap(float2 texCoords) {
	return 1.0f - HeightMap.Sample(MaterialSampler, texCoords).r;
}

float2 ParallaxMapping(float2 texCoords, float3 viewDir) {
	const float MinLayers = 32;
	const float MaxLayers = 64;

	float NumLayers = lerp(MaxLayers, MinLayers, abs(dot(float3(0.0f, 0.0f, 1.0f), viewDir)));
	float LayerDepth = 1.0f / NumLayers;

	float2 P = viewDir.xy / viewDir.z * HEIGHT_SCALE;
	float2 DeltaTexCoords = P / NumLayers;

	float2 CurrentTexCoords = texCoords;
	float CurrentDepthMapValue = SampleHeightMap(CurrentTexCoords);

	float CurrentLayerDepth = 0.0f;
	while (CurrentLayerDepth < CurrentDepthMapValue) {
		CurrentTexCoords -= DeltaTexCoords;
		CurrentDepthMapValue = SampleHeightMap(CurrentTexCoords);
		CurrentLayerDepth += LayerDepth;
	}

	float2 PrevTexCoords = CurrentTexCoords + DeltaTexCoords;

	float AfterDepth = CurrentDepthMapValue - CurrentLayerDepth;
	float BeforeDepth = SampleHeightMap(PrevTexCoords) - CurrentLayerDepth + LayerDepth;

	float Weight = AfterDepth / (AfterDepth - BeforeDepth);
	float2 FinalTexCoords = PrevTexCoords * Weight + CurrentTexCoords * (1.0f - Weight);

	return FinalTexCoords;
}

#endif

VSOutput VSMain(VSInput input) {
	VSOutput output;

	const float4x4 transformInverse = TransformBuffer.TransformInverse;
	float3 normal = normalize(mul(float4(input.Normal, 0.0f), transformInverse).xyz);
	output.Normal = normal;

	float3 viewNormal = mul(float4(normal, 0.0f), CameraBuffer.View).xyz;
	output.ViewNormal = viewNormal;

#if defined(NORMAL_MAPPING_ENABLED) || defined(PARALLAX_MAPPING_ENABLED)
	float3 tangent = normalize(mul(float4(input.Tangent, 0.0f), transformInverse).xyz);
	tangent = normalize(tangent - dot(tangent, normal) * normal);
	output.Tangent = tangent;

	float3 bitangent = normalize(cross(tangent, normal));
	output.Bitangent = bitangent;
#endif

	output.TexCoord = input.TexCoord;

	float4 worldPosition = mul(float4(input.Position, 1.0f), TransformBuffer.Transform);
	output.Position = mul(worldPosition, CameraBuffer.ViewProjection);

#ifdef PARALLAX_MAPPING_ENABLED
	float3x3 TBN = float3x3(tangent, bitangent, normal);
	output.TangentViewPos = mul(TBN, CameraBuffer.Position);
	output.TangentPosition = mul(TBN, worldPosition.xyz);
#endif

	return output;
}

PSOutput PSMain(PSInput input) {
	float2 texCoords = input.TexCoord;
	texCoords.y = 1.0f - texCoords.y;

#ifdef PARALLAX_MAPPING_ENABLED
	if (MaterialBuffer.EnableHeight != 0) {
		float3 viewDir = normalize(input.TangentViewPos - input.TangentPosition);
		texCoords = ParallaxMapping(texCoords, viewDir);
		if (texCoords.x > 1.0f || texCoords.y > 1.0f || texCoords.x < 0.0f || texCoords.y < 0.0f) {
			discard;
		}
	}
#endif

	float3 SampledAlbedo = ApplyGamma(AlbedoMap.Sample(MaterialSampler, texCoords).rgb) * MaterialBuffer.Albedo;

#ifdef NORMAL_MAPPING_ENABLED
	float3 SampledNormal = NormalMap.Sample(MaterialSampler, texCoords).rgb;
	SampledNormal = UnpackNormal(SampledNormal);
	SampledNormal.y = - SampledNormal.y;

	float3 Tangent = normalize(input.Tangent);
	float3 Bitangent = normalize(input.Bitangent);
	float3 Normal = normalize(input.Normal);
	float3 MappedNormal = ApplyNormalMapping(SampledNormal, Normal, Tangent, Bitangent);
#else
	float3 MappedNormal = input.Normal;
#endif
	MappedNormal = PackNormal(MappedNormal);

	const float SampledAO = AOMap.Sample(MaterialSampler, texCoords).r * MaterialBuffer.AO;
	const float SampledMetallic = MetallicMap.Sample(MaterialSampler, texCoords).r * MaterialBuffer.Metallic;
	const float SampledRoughness = RoughnessMap.Sample(MaterialSampler, texCoords).r * MaterialBuffer.Roughness;
	const float FinalRoughness = min(max(SampledRoughness, MIN_ROUGHNESS), MAX_ROUGHNESS);

	PSOutput output;
	output.Albedo = float4(SampledAlbedo, 1.0f);
	output.Normal = float4(MappedNormal, 1.0f);
	output.Material = float4(FinalRoughness, SampledMetallic, SampledAO, 1.0f);
	output.ViewNormal = float4(PackNormal(input.ViewNormal), 1.0f);

	return output;
}
