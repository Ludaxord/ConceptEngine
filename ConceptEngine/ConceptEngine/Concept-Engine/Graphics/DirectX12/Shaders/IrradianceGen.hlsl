#include "PBRHelpers.hlsli"

#define BLOCK_SIZE 16

#define RootSig "RootFlags(0), RootConstants(b0, num32BitConstants = 1), DescriptorTable(SRV(t0, numDescriptors = 1)), DescriptorTable(UAV(u0, numDescriptors = 1)), StaticSampler(s0, addressU = TEXTURE_ADDRESS_WRAP, addressV = TEXTURE_ADDRESS_WRAP, addressW = TEXTURE_ADDRESS_WRAP, filter = FILTER_MIN_MAG_LINEAR_MIP_POINT)"

TextureCube<float4> EnvironmentMap : register(t0, space0);
SamplerState EnvironmentSampler : register(s0, space0);

RWTexture2DArray<float4> IrradianceMap : register(u0);

static const float3x3 RotateUV[6] =
{
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
          uint3 DispatchThreadID : SV_DispatchThreadID, uint GroupIndex : SV_GroupIndex) {

	uint3 texCoord = DispatchThreadID;

	uint width;
	uint height;
	uint elements;
	IrradianceMap.GetDimensions(width, height, elements);

	float3 normal = float3((texCoord.xy / float(width)) - 0.5f, 0.5f);
	normal = normalize(mul(RotateUV[texCoord.z], normal));

	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 right = cross(up, normal);
	up = cross(normal, right);

	float sampleDelta = 0.025f;
	float nrSamples = 0.0f;
	float3 irradiance = float3(0.0f, 0.0f, 0.0f);
	for (float phi = 0.0f; phi < 2.0f * PI; phi += sampleDelta) {
		for (float theta = 0.0f; theta < 0.5f * PI; theta += sampleDelta) {
			float3 tangentSample = float3(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
			float3 sampleVec = tangentSample.x * right + tangentSample.y * up + tangentSample.z * normal;

			irradiance += EnvironmentMap.SampleLevel(EnvironmentSampler, sampleVec, 0).rgb * cos(theta) * sin(theta);
			nrSamples += 1.0f;
		}
	}

	irradiance = PI * irradiance * (1.0f / float(nrSamples));
	IrradianceMap[texCoord] = float4(irradiance, 1.0f);
}
