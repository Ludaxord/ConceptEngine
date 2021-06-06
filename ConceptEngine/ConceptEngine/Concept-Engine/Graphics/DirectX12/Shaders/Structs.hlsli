#ifndef STRUCTS_HLSLI
#define STRUCTS_HLSLI

struct ComputeShaderInput {
	uint3 GroupID : SV_GroupID;
	uint3 GroupThreadID : SV_GroupThreadID;
	uint3 DispatchThreadID : SV_DispatchThreadID;
	uint GroupIndex : SV_GroupIndex;
};

#endif
