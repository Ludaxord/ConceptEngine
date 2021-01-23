#define BLOCK_SIZE 8

/*
 * When reducing size of texture, it could be that downScaling texture will result in a less than exactly 50% (1/2) of original texture size.
 * This happens if either width or height (or both) dimensions of texture are odd. For example,
 * downscaling a 5x3 texture will result in 2x1 texture which has 60% reduction in texture width and 66% reduction in height.
 * When this happens, wee need to take more samples from source texture to determine pixel value in destination texture.
 */

/*
 * Both width and height of texture are even.
 */
#define WIDTH_EVEN_HEIGHT_EVEN 0
/*
 * texture width is odd and height is even
 */
#define WIDTH_ODD_HEIGHT_EVEN 1
/*
 * texture width is even and height is odd
 */
#define WIDTH_EVEN_HEIGHT_ODD 2
/*
 * Both width and height of texture are odd.
 */
#define WIDTH_ODD_HEIGHT_ODD 3


struct ComputeShaderInput {
	/*
	 * 3D index of thread group in dispatch.
	 */
	uint3 GroupID: SV_GroupID;
	/*
	 * 3D index of local thread ID in thread group.
	 */
	uint3 GroupThreadID: SV_GroupThreadID;
	/*
	 * 3D index of global thread ID in dispatch.
	 */
	uint3 DispatchThreadID: SV_DispatchThreadID;
	/*
	 * Flattened local index of thread within a thread group.
	 */
	uint GroupIndex: SV_GroupIndex;
};

cbuffer GenerateMipsCB : register(b0) {
/*
 * Texture level of source mip
 */
uint SrcMipLevel;
/*
 * Number of OutMips to write: [1-4]
 */
uint NumMipLevels;
/*
 * Width and height of source texture are event or odd.
 */
uint SrcDimension;
/*
 * Must apply gamma correction to sRGB textures.
 */
bool IsSRGB;
/*
 * 1.0 / OutMip1.Dimensions
 */
float2 TexelSize;
}

/*
 * Source mip map
 */
Texture2D<float4> SrcMip: register(t0);

/*
 * Write up to 4 mip map levels.
 */
RWTexture2D<float4> OutMip1 : register(u0);
RWTexture2D<float4> OutMip2 : register(u1);
RWTexture2D<float4> OutMip3 : register(u2);
RWTexture2D<float4> OutMip4 : register(u3);

/*
 * Linear clamp sampler
 */
SamplerState LinearClampSampler : register(s0);

#define GenerateMips_RootSignature \
    "RootFlags(0), " \
    "RootConstants(b0, num32BitConstants = 6), " \
    "DescriptorTable( SRV(t0, numDescriptors = 1) )," \
    "DescriptorTable( UAV(u0, numDescriptors = 4) )," \
    "StaticSampler(s0," \
        "addressU = TEXTURE_ADDRESS_CLAMP," \
        "addressV = TEXTURE_ADDRESS_CLAMP," \
        "addressW = TEXTURE_ADDRESS_CLAMP," \
        "filter = FILTER_MIN_MAG_MIP_LINEAR)"

/*
 * reason for separating channels is to reduce bank conflicts in local data memory controller.
 * Large stride will cause more threads to collide on same memory bank
 */
groupshared float gs_R[64];
groupshared float gs_G[64];
groupshared float gs_B[64];
groupshared float gs_A[64];

void StoreColor(uint Index, float4 Color) {
	gs_R[Index] = Color.r;
	gs_G[Index] = Color.g;
	gs_B[Index] = Color.b;
	gs_A[Index] = Color.a;
}

float4 LoadColor(uint Index) {
	return float4(gs_R[Index], gs_G[Index], gs_B[Index], gs_A[Index]);
}

/*
 * Source: https://en.wikipedia.org/wiki/SRGB#The_reverse_transformation
 */
float3 ConvertToLinear(float3 x) {
	return x < 0.04045f ? x / 12.92 : pow((x + 0.055) / 1.055, 2.4);
}

/*
 * Source: https://en.wikipedia.org/wiki/SRGB#The_forward_transformation_(CIE_XYZ_to_sRGB)
 */
float3 ConvertToSRGB(float3 x) {
	return x < 0.0031308 ? 12.02 * x : 1.055 * pow(abs(x), 1.0 / 2.4) - 0.055;
}

/*
 * Convert linear color to sRGB before storing if original source is an sRGB texture.
 */
float4 PackColor(float4 x) {
	if (IsSRGB) {
		return float4(ConvertToLinear(x.rgb), x.a);
	}
	else {
		return x;
	}
}

[RootSignature(GenerateMips_RootSignature)]
[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void main(ComputeShaderInput IN) {
	float4 Src1 = (float4)0;

	/*
	 * One bilinear sample is insufficient when scaling down by more than 2x.
	 * You will slighty undersample in case where source dimension is odd.
	 * This is why it is a really good idea to only generate mips on power-of-two sized textures.
	 * Trying to handle undersampling case will force shader to sbe slower
	 * and more complicated as it will have to take more source texture samples
	 */

	/*
	 * Determine path to use based on dimension of source texture.
	 * source texture.
	 * 0b00(0): Both width and height are even.
	 * 0b01(1): Width is odd, height is even.
	 * 0b10(2): Width is even, height is odd.
	 * 0b11(3): Both width and height are odd.
	 */
	switch (SrcDimension) {
	case WIDTH_EVEN_HEIGHT_EVEN: {
		float2 UV = TexelSize * (IN.DispatchThreadID.xy + 0.5);
		Src1 = SrcMip.SampleLevel(LinearClampSampler, UV, SrcMipLevel);
	}
	break;
	case WIDTH_ODD_HEIGHT_EVEN: {
		/*
		 * > 2:1 in X dimension
		 * Use 2 bilinear samples to guarantee that it will not be undersampled when downsizing by more than 2x horizontally.
		 */
		float2 UV1 = TexelSize * (IN.DispatchThreadID.xy + float2(0.25, 0.5));
		float2 Off = TexelSize * float2(0.5, 0.0);

		Src1 = 0.5 * (SrcMip.SampleLevel(LinearClampSampler, UV1, SrcMipLevel) +
			SrcMip.SampleLevel(LinearClampSampler, UV1 + Off, SrcMipLevel));
	}
	break;
	case WIDTH_EVEN_HEIGHT_ODD: {
		/*
		 * > 2:1 in Y dimension
		 * Use 2 bilinear samples to guarantee that it will not be undersampled when downsizing by more than 2x vertically.
		 */
		float2 UV1 = TexelSize * (IN.DispatchThreadID.xy + float2(0.5, 0.25));
		float2 Off = TexelSize * float2(0.0, 0.5);

		Src1 = 0.5 * (SrcMip.SampleLevel(LinearClampSampler, UV1, SrcMipLevel) +
			SrcMip.SampleLevel(LinearClampSampler, UV1 + Off, SrcMipLevel));
	}
	break;
	case WIDTH_ODD_HEIGHT_ODD: {
		/*
		 * > 2:1 in both dimensions
		 * Use 4 bilinear samples to guarantee it will not be undersampled when downsizing by more than 2x in both directions.
		 */
		float2 UV1 = TexelSize * (IN.DispatchThreadID.xy + float2(0.25, 0.5));
		float2 Off = TexelSize * 0.5;

		Src1 = SrcMip.SampleLevel(LinearClampSampler, UV1, SrcMipLevel);
		Src1 += SrcMip.SampleLevel(LinearClampSampler, UV1 + float2(Off.x, 0.0), SrcMipLevel);
		Src1 += SrcMip.SampleLevel(LinearClampSampler, UV1 + float2(0.0, Off.y), SrcMipLevel);
		Src1 += SrcMip.SampleLevel(LinearClampSampler, UV1 + float2(Off.x, Off.y), SrcMipLevel);
		Src1 *= 0.25;
	}
	break;
	}

	OutMip1[IN.DispatchThreadID.xy] = PackColor(Src1);

	/*
	 * Scalar (constant) branch can exit all threads coherently
	 */
	if (NumMipLevels == 1)
		return;
	/*
	 * Without lane swizzle operations, only way to share data with other threads is through LDS.
	 */
	StoreColor(IN.GroupIndex, Src1);

	/*
	 * Guarantees all LDS writes are complete and that all threads have executed all instructions so far (and therefore have issued their LDS write instructions);
	 */
	GroupMemoryBarrierWithGroupSync();

	/*
	 * With low three bits for X and high three bits for Y, this bit mask (binary: 001001) checks that X and Y are even;
	 */
	if ((IN.GroupIndex & 0x9) == 0) {
		float4 Src2 = LoadColor(IN.GroupIndex + 0x01);
		float4 Src3 = LoadColor(IN.GroupIndex + 0x08);
		float4 Src4 = LoadColor(IN.GroupIndex + 0x09);
		Src1 = 0.25 * (Src1 + Src2 + Src3 + Src4);

		OutMip2[IN.DispatchThreadID.xy / 2] = PackColor(Src1);
		StoreColor(IN.GroupIndex, Src1);
	}

	if (NumMipLevels == 2)
		return;

	GroupMemoryBarrierWithGroupSync();

	/*
	 * This bit mask (binary: 011011) check that X and Y are multiples of four
	 */
	if ((IN.GroupIndex & 0x1B) == 0) {
		float4 Src2 = LoadColor(IN.GroupIndex + 0x02);
		float4 Src3 = LoadColor(IN.GroupIndex + 0x10);
		float4 Src4 = LoadColor(IN.GroupIndex + 0x12);
		Src1 = 0.25 * (Src1 + Src2 + Src3 + Src4);

		OutMip3[IN.DispatchThreadID.xy / 4] = PackColor(Src1);
		StoreColor(IN.GroupIndex, Src1);
	}

	if (NumMipLevels == 3)
		return;

	GroupMemoryBarrierWithGroupSync();

	/*
	 * This bit mask would be 111111 (X & Y multiples of 8), but only one thread firs that criteria
	 */
	if (IN.GroupIndex == 0) {
		float Src2 = LoadColor(IN.GroupIndex + 0x04);
		float Src3 = LoadColor(IN.GroupIndex + 0x20);
		float Src4 = LoadColor(IN.GroupIndex + 0x24);
		Src1 = 0.25 * (Src1 + Src2 + Src3 + Src4);

		OutMip4[IN.DispatchThreadID.xy / 8] = PackColor(Src1);
	}
}
