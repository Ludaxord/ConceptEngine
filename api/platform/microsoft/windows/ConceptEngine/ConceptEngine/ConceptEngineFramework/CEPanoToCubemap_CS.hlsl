/*
 * Compute shader is used to convert a panoramic (equirectangular) image into cubemap.
 */

#define BLOCK_SIZE 16

struct ComputeShaderInput {
	/*
	 * 3D Index of Thread group in dispatch.
	 */
	uint3 GroupID : SV_GroupID;
	/*
	 * 3D index of local thread ID in a thread group.
	 */
	uint3 GroupThreadID : SV_GroupThreadID;
	/*
	 * 3D index of global thread ID in dispatch
	 */
	uint3 DispatchThreadID : SV_DispatchThreadID;
	/*
	 * Flattened local index of thread within a thread group
	 */
	uint GroupIndex : SV_GroupIndex;
};

struct PanoToCubemap {
	/*
	 * Size of cubemap face in pixels at current mipmap level;
	 */
	uint CubemapSize;
	/*
	 * First mip level to generate.
	 */
	uint FirstMip;
	/*
	 * Number of mips to generate
	 */
	uint NumMips;
};

ConstantBuffer<PanoToCubemap> PanoToCubemapCB : register(b0);

/*
 * Source texture as an equirectangular panoramic image.
 * It is assumed that source texture has full mipmap chain.
 */
Texture2D<float4> SrcTexture : register(t0);

/*
 * Destination texture as mip slice in cubemap texture (texture array with 6 elements)
 */
RWTexture2DArray<float4> DstMip1 : register(u0);
RWTexture2DArray<float4> DstMip2 : register(u1);
RWTexture2DArray<float4> DstMip3 : register(u2);
RWTexture2DArray<float4> DstMip4 : register(u3);
RWTexture2DArray<float4> DstMip5 : register(u4);

/*
 * Linear repeat sampler.
 */
SamplerState LinearRepeatSampler : register(s0);

#define GenerateMips_RootSignature \
    "RootFlags(0), " \
    "RootConstants(b0, num32BitConstants = 3), " \
    "DescriptorTable( SRV(t0, numDescriptors = 1) )," \
    "DescriptorTable( UAV(u0, numDescriptors = 5) )," \
    "StaticSampler(s0," \
        "addressU = TEXTURE_ADDRESS_WRAP," \
        "addressV = TEXTURE_ADDRESS_WRAP," \
        "addressW = TEXTURE_ADDRESS_WRAP," \
        "filter = FILTER_MIN_MAG_LINEAR_MIP_POINT )"

/*
 * 1 / PI
 */
static const float InvPI = 0.31830988618379067153776752674503f;
static const float Inv2PI = 0.15915494309189533576888376337251f;
static const float2 InvAtan = float2(Inv2PI, InvPI);

/*
 * Transform from dispatch ID to cubemap face direction
 */
static const float3x3 RotateUV[6] = {
	/*
	 * +X
	 */
	float3x3(0, 0, 1,
	         0, -1, 0,
	         -1, 0, 0),
	/*
	 * -X
	 */
	float3x3(0, 0, -1,
	         0, -1, 0,
	         1, 0, 0),
	/*
	 * +Y
	 */
	float3x3(1, 0, 0,
	         0, 0, 1,
	         0, 1, 0),
	/*
	 * -Y
	 */
	float3x3(1, 0, 0,
	         0, 0, -1,
	         0, -1, 0),
	/*
	 * +Z
	 */
	float3x3(1, 0, 0,
	         0, -1, 0,
	         0, 0, 1),
	/*
	 * -Z
	 */
	float3x3(-1, 0, 0,
	         0, -1, 0,
	         0, 0, -1)
};


[RootSignature(GenerateMips_RootSignature)]
[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void main(ComputeShaderInput IN) {
	/*
	 * Cubemap texture coords
	 */
	uint3 texCoord = IN.DispatchThreadID;

	/*
	 * First check if thread is in cubemap dimensions
	 */
	if (texCoord.x >= PanoToCubemapCB.CubemapSize || texCoord.y >= PanoToCubemapCB.CubemapSize)
		return;
	/*
	 * Map to UV coords of cubemap face to direction
	 * [(0,0), (1,1)] => [(-0.5, -0.5), (0.5, 0,5)]
	 */
	float3 dir = float3(texCoord.xy / float(PanoToCubemapCB.CubemapSize) - 0.5f, 0.5f);

	/*
	 * Rotate cubemap face
	 */
	dir = normalize(mul(RotateUV[texCoord.z], dir));

	/*
	 * Convert world space direction into U,V texture coordinates in panoramic texture.
	 */
	float2 panoUV = float2(atan2(-dir.x, -dir.z), acos(dir.y)) * InvAtan;

	DstMip1[texCoord] = SrcTexture.SampleLevel(LinearRepeatSampler, panoUV, PanoToCubemapCB.FirstMip);

	/*
	 * Only perform on threads that are a multiple of 2.
	 */
	if (PanoToCubemapCB.NumMips > 1 && (IN.GroupIndex & 0x11) == 0) {
		DstMip2[uint3(texCoord.xy / 2, texCoord.z)] = SrcTexture.SampleLevel(
			LinearRepeatSampler, panoUV, PanoToCubemapCB.FirstMip + 1);
	}

	/*
	 * Only perform on threads that are a multiple of 4.
	 */
	if (PanoToCubemapCB.NumMips > 2 && (IN.GroupIndex & 0x33) == 0) {
		DstMip2[uint3(texCoord.xy / 4, texCoord.z)] = SrcTexture.SampleLevel(
			LinearRepeatSampler, panoUV, PanoToCubemapCB.FirstMip + 2);
	}

	/*
	 * Only perform on threads that are a multiple of 8.
	 */
	if (PanoToCubemapCB.NumMips > 3 && (IN.GroupIndex & 0x77) == 0) {
		DstMip2[uint3(texCoord.xy / 8, texCoord.z)] = SrcTexture.SampleLevel(
			LinearRepeatSampler, panoUV, PanoToCubemapCB.FirstMip + 3);
	}

	/*
	 * Only perform on threads that are a multiple of 16.
	 * This should only be thread 0 in this group.
	 */
	if (PanoToCubemapCB.NumMips > 4 && (IN.GroupIndex & 0xFF) == 0) {
		DstMip2[uint3(texCoord.xy / 16, texCoord.z)] = SrcTexture.SampleLevel(
			LinearRepeatSampler, panoUV, PanoToCubemapCB.FirstMip + 4);
	}
}
