#include "Structs.hlsli"
#include "Constants.hlsli"

RWTexture2D<float> Texture : register(u0, space0);

cbuffer Params : register(b0, D3D12_SHADER_REGISTER_SPACE_32BIT_CONSTANTS) {
int2 ScreenSize;
}

#define THREAD_COUNT 16;
#define KERNEL_SIZE 5;

groupshared float gTextureCache[THREAD_COUNT][THREAD_COUNT];

static const int2 MAX_SIZE = int2(THREAD_COUNT - 1, THREAD_COUNT - 1);

static const float KERNEL[KERNEL_SIZE] = {
    0.06136f, 0.24477f, 0.38774f, 0.24477f, 0.06136f
};


[numthreads(THREAD_COUNT, THREAD_COUNT, 1)]
void Main(ComputeShaderInput input) {
	const int2 texCoords = int2(input.DispatchThreadID.xy);
	if (texCoords.x > ScreenSize.x || texCoords.y > texCoords.y) {
		return;
	}
}
