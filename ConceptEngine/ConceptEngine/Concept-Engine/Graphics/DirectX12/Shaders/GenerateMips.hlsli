#include "Structs.hlsli"
#include "Constants.hlsli"

#define BLOCK_SIZE 8
#define CHANNEL_COMPONENTS BLOCK_SIZE * BLOCK_SIZE
#define POWER_OF_TWO 1

#define RootSig "RootFlags(0), RootConstants(b0, space = 1, num32BitConstants = 4), DescriptorTable(SRV(t0, numDescriptors = 1)), DescriptorTable(UAV(u0, numDescriptors = 4)), StaticSampler(s0, addressU = TEXTURE_ADDRESS_WRAP, addressV = TEXTURE_ADDRESS_WRAP, addressW = TEXTURE_ADDRESS_WRAP, filter = FILTER_MIN_MAG_MIP_LINEAR)"

#if CUBE_MAP
TextureCube<float4> SourceMip : register(t0);
#else
Texture2D<float4> SourceMip : register(t0);
#endif

#if CUBE_MAP
RWTexture2DArray<float4> OutMip1 : register(u0);
RWTexture2DArray<float4> OutMip2 : register(u1);
RWTexture2DArray<float4> OutMip3 : register(u2);
RWTexture2DArray<float4> OutMip4 : register(u3);
#else
RWTexture2D<float4> OutMip1 : register(u0);
RWTexture2D<float4> OutMip2 : register(u1);
RWTexture2D<float4> OutMip3 : register(u2);
RWTexture2D<float4> OutMip4 : register(u3);
#endif

SamplerState LinearSampler : register(s0);

cbuffer CB0 : register(b0, D3D12_SHADER_REGISTER_SPACE_32BIT_CONSTANTS) {
uint SrcMipLevel;
uint NumMipLevels;
float2 TexelSize;
}

groupshared float RedChannel[CHANNEL_COMPONENTS];
groupshared float GreenChannel[CHANNEL_COMPONENTS];
groupshared float BlueChannel[CHANNEL_COMPONENTS];
groupshared float AlphaChannel[CHANNEL_COMPONENTS];

void StoreColor(uint index, float4 color) {
	RedChannel[index] = color.r;
	GreenChannel[index] = color.g;
	BlueChannel[index] = color.b;
	AlphaChannel[index] = color.a;
}

float4 LoadColor(uint index) {
	return float4(RedChannel[index], GreenChannel[index], BlueChannel[index], AlphaChannel[index]);
}

float3 ApplySRGBCurve(float3 x) {
	return x < 0.0031308f ? 12.92f * x : 1.13005f * sqrt(abs(x - 0.00228f)) - 0.13448f * x + 0.005719f;
}

float4 PackColor(float4 Linear) {
#ifdef CONVERT_TO_SRGB
	return float4(ApplySRGBCurve(Linear.rgb), Linear.a);
#else
	return Linear;
#endif
}

#if CUBE_MAP
// Transform from dispatch ID to cubemap face direction
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
#endif

[RootSignature(RootSig)]
[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void Main(ComputeShaderInput input) {

#if CUBE_MAP
	float3 TexCoord = float3((input.DispatchThreadID.xy * TexelSize) - 0.5f, 0.5f);
	TexCoord = normalize(mul(RotateUV[input.DispatchThreadID.z], TexCoord));
	float4 Src1 = SourceMip.SampleLevel(LinearSampler, TexCoord, SrcMipLevel);
#else
#if POWER_OF_TWO
        float2 TexCoord = TexelSize * (Input.DispatchThreadID.xy + 0.5f);
        float4 Src1		= SourceMip.SampleLevel(LinearSampler, TexCoord, SrcMipLevel);
#else
        #error "Not supported yet"
#endif
#endif

#if CUBE_MAP
	OutMip1[input.DispatchThreadID] = PackColor(Src1);
#else
	OutMip1[input.DispatchThreadID.xy] = PackColor(Src1);
#endif

	if (NumMipLevels == 1) {
		return;
	}

	StoreColor(input.GroupIndex, Src1);

	GroupMemoryBarrierWithGroupSync();

	if ((input.GroupIndex & 0x9) == 0) {
		float4 Src2 = LoadColor(input.GroupIndex + 0x01);
		float4 Src3 = LoadColor(input.GroupIndex + 0x08);
		float4 Src4 = LoadColor(input.GroupIndex + 0x09);
		Src1 = 0.25f * (Src1 + Src2 + Src3 + Src4);

#if CUBE_MAP
		OutMip2[uint3(input.DispatchThreadID.xy / 2, input.DispatchThreadID.z)] = PackColor(Src1);
#else
		OutMip2[input.DispatchThreadID.xy / 2] = PackColor(Src1);
#endif
		StoreColor(input.GroupIndex, Src1);
	}

	if (NumMipLevels == 2) {
		return;
	}

	GroupMemoryBarrierWithGroupSync();

	if ((input.GroupIndex & 0x1B) == 0) {
		float4 Src2 = LoadColor(input.GroupIndex + 0x02);
		float4 Src3 = LoadColor(input.GroupIndex + 0x10);
		float4 Src4 = LoadColor(input.GroupIndex + 0x12);
		Src1 = 0.25f * (Src1 + Src2 + Src3 + Src4);

#if CUBE_MAP
		OutMip3[uint3(input.DispatchThreadID.xy / 4, input.DispatchThreadID.z)] = PackColor(Src1);
#else
		OutMip3[input.DispatchThreadID.xy / 4] = PackColor(Src1);
#endif
		StoreColor(input.GroupIndex, Src1);
	}

	if (NumMipLevels == 3) {
		return;
	}

	GroupMemoryBarrierWithGroupSync();

	if (input.GroupIndex == 0) {
		float4 Src2 = LoadColor(input.GroupIndex + 0x04);
		float4 Src3 = LoadColor(input.GroupIndex + 0x20);
		float4 Src4 = LoadColor(input.GroupIndex + 0x24);
		Src1 = 0.25f * (Src1 + Src2 + Src3 + Src4);

#if CUBE_MAP
		OutMip4[uint3(input.DispatchThreadID.xy / 8, input.DispatchThreadID.z)] = PackColor(Src1);
#else
		OutMip4[input.DispatchThreadID.xy / 8] = PackColor(Src1);
#endif
	}
}
