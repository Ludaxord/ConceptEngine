#include "PBRHelpers.hlsli"
#include "Constants.hlsli"

#define BLOCK_SIZE 16

#define RootSig "RootFlags(0), RootConstants(b0, space = 1, num32BitConstants = 1), DescriptorTable(SRV(t0, numDescriptors = 1)), DescriptorTable(UAV(u0, numDescriptors = 1)), StaticSampler(s0, addressU = TEXTURE_ADDRESS_WRAP, addressV = TEXTURE_ADDRESS_WRAP, addressW = TEXTURE_ADDRESS_WRAP, filter = FILTER_MIN_MAG_MIP_LINEAR)"

cbuffer CB0 : register(b0, D3D12_SHADER_REGISTER_SPACE_32BIT_CONSTANTS) {
float Roughness;
}

TextureCube<float4> EnvironmentMap : register(t0, space0);
SamplerState EnvironmentSampler : register(s0, space0);

RWTexture2DArray<float4> SpecularIrradianceMap : register(u0, space0);

static const float3x3 RotateUV[6] = {
	// +X
	float3x3(0, 0, 1,
	         0, -1, 0,
	         -1, 0, 0),
	// -X
	float3x3(0, 0, -1,
	         0, -1, 0,
	         1, 0, 0),
	// +Y     
	float3x3(1, 0, 0,
	         0, 0, 1,
	         0, 1, 0),
	// -Y     
	float3x3(1, 0, 0,
	         0, 0, -1,
	         0, -1, 0),
	// +Z     
	float3x3(1, 0, 0,
	         0, -1, 0,
	         0, 0, 1),
	// -Z
	float3x3(-1, 0, 0,
	         0, -1, 0,
	         0, 0, -1)
};

[RootSignature(RootSig)]
[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void Main(uint3 GroupID : SV_GroupID, uint3 GroupThreadID : SV_GroupThreadID,
          uint3 DispatchThreadID : SV_DispatchThreadID,
          uint GroupIndex : SV_GroupIndex) {
	uint3 texCoord = DispatchThreadID;

	uint width;
	uint height;
	uint elements;
	SpecularIrradianceMap.GetDimensions(width, height, elements);

	uint sourceWidth;
	uint sourceHeight;
	EnvironmentMap.GetDimensions(sourceWidth, sourceHeight);

	float3 normal = float3((texCoord.xy / float(width)) - 0.5f, 0.5f);
	normal = normalize(mul(RotateUV[texCoord.z], normal));

	float3 R = normal;
	float3 V = R;

	float finalRoughness = min(max(Roughness, MIN_ROUGHNESS), MAX_ROUGHNESS);
	float totalWeight = 0.0f;
	float3 prefilteredColor = float3(0.0f, 0.0f, 0.0f);

	const uint SAMPLE_COUNT = 512U;
	for (uint i = 0U; i < SAMPLE_COUNT; i++) {
		float2 Xi = Hammersley(i, SAMPLE_COUNT);
		float3 H = ImportanceSampleGGX(Xi, finalRoughness, normal);
		float3 L = normalize(2.0 * dot(V, H) * H - V);

		float NDotL = max(dot(normal, L), 0.0f);
		if (NDotL > 0.0f) {
			float D = DistributionGGX(normal, H, finalRoughness);
			float NDotH = max(dot(normal, H), 0.0f);
			float HDotV = max(dot(H, V), 0.0f);
			float PDF = D * NDotH / (4.0f * HDotV) + 0.0001f;

			float resolution = float(sourceWidth);
			float saTexel = 4.0f * PI / (6.0f * resolution * resolution);
			float saSample = 1.0f / (float(SAMPLE_COUNT) * PDF + 0.0001f);

			const float mipLevel = finalRoughness == 0.0f ? 0.0f : 0.5f * log2(saSample / saTexel);

			prefilteredColor += EnvironmentMap.SampleLevel(EnvironmentSampler, L, mipLevel).rgb * NDotL;
			totalWeight += NDotL;
		}
	}

	prefilteredColor = prefilteredColor / totalWeight;
	SpecularIrradianceMap[texCoord] = float4(prefilteredColor, 1.0f);
}
