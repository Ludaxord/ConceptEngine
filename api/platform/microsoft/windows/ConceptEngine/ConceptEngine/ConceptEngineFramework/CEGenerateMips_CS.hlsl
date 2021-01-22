#define BLOCK_SIZE 8

/*
 * When reducing size of texture, it could be that downScaling texture will result in a less than exactly 50%
 */


struct ComputeShaderInput {
	uint3 GroupID: SV_GroupID;
	uint3 GroupThreadID: SV_GroupThreadID;
	uint3 DispatchThreadID: SV_DispatchThreadID;
	uint GroupIndex: SV_GroupIndex;
};

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

[RootSignature(GenerateMips_RootSignature)]
[numthreads(BLOCK_SIZE, BLOCK_SIZE, 1)]
void main(ComputeShaderInput IN) {
}
