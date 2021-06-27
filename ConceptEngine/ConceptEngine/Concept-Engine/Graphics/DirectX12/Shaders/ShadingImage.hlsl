#include "Structs.hlsli"
#include "Constants.hlsli"

RWTexture2D<uint> output : register(u0, space0);

[numthreads(1, 1, 1)]
void Main(ComputeShaderInput input) {
	uint width;
	uint height;
	output.GetDimensions(width, height);

	int2 texCoord = int2(input.DispatchThreadID.xy);
	float2 center = float2(width / 2, height / 2);
	float distance = length(float2(texCoord) - center);

	if (distance > 40.0f) {
		output[texCoord] = SHADING_RATE_4x4;
	}
	else if (distance > 12.0f) {
		output[texCoord] = SHADING_RATE_2x2;
	}
	else {
		output[texCoord] = SHADING_RATE_1x1;
	}
}
