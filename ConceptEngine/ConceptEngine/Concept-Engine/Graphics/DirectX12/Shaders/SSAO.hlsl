#include "PBRHelpers.hlsli"
#include "Structs.hlsli"

Texture2D<float3> BufferNormals : register(t0, space0);
Texture2D<float> BufferDepth : register(t1, space0);
Texture2D<float3> Noise : register(t2, space0);

StructuredBuffer<float3> Samples : register(t3, space0);

SamplerState BufferSampler : register(s0, space0);
SamplerState NoiseSampler : register(s1, space1);

RWTexture2D<float> Output : register(u0, space0);

cbuffer Params : register(b0, D3D12_SHADER_REGISTER_SPACE_32BIT_CONSTANTS) {
float2 ScreenSize;
float2 NoiseSize;
float Radius;
float Bias;
int KernelSize;
}

ConstantBuffer<Camera> CameraBuffer : register(b0, space0);

#define THREAD_COUNT 16
#define MAX_SAMPLES 64

[numthreads(THREAD_COUNT, THREAD_COUNT, 1)]
void Main(ComputeShaderInput input) {
	const uint2 outputTexCoords = input.DispatchThreadID.xy;
	if (input.DispatchThreadID.x > uint(ScreenSize.x) || input.DispatchThreadID.y > uint(ScreenSize.y)) {
		return;
	}

	const float2 texCoords = (float2(outputTexCoords) + 0.5f) / ScreenSize;
	const float depth = BufferDepth.SampleLevel(BufferSampler, texCoords, 0);
	if (depth >= 1.0f) {
		Output[outputTexCoords] = 1.0f;
		return;
	}

	float3 viewPosition = PositionFromDepth(depth, texCoords, CameraBuffer.ProjectionInverse);

	float3 viewNormal = BufferNormals.SampleLevel(BufferSampler, texCoords, 0).rgb;
	viewNormal = UnpackNormal(viewNormal);

	const float2 noiseScale = ScreenSize / NoiseSize;
	const float3 noiseVec = normalize(Noise.SampleLevel(NoiseSampler, texCoords * noiseScale, 0));
	const float3 tangent = normalize(noiseVec - viewNormal * dot(noiseVec, viewNormal));
	const float3 bitangent = cross(viewNormal, tangent);
	float3x3 TBN = float3x3(tangent, bitangent, viewNormal);

	const float finalRadius = max(Radius, 0.05f);
	const float finalBias = max(Bias, 0.0f);
	const int finalKernelSize = max(KernelSize, 4);

	float occlusion = 0.0f;
	for (int i = 0; i < finalKernelSize; i++) {
		const int index = int(float(MAX_SAMPLES) * Random(floor(viewPosition.xyz * 1000.0f), i));

		const float3 Sample = Samples[index];
		float3 samplePos = mul(Sample, TBN);
		samplePos = viewPosition + samplePos * finalRadius;

		float4 offset = mul(CameraBuffer.Projection, float4(samplePos, 1.0f));
		offset.xy = offset.xy / offset.w;
		offset.xy = (offset.xy * 0.5f) + 0.5f;
		offset.y = 1.0f - offset.y;

		float sampleDepth = BufferDepth.SampleLevel(BufferSampler, offset.xy, 0);
		float3 depthPos = PositionFromDepth(sampleDepth, offset.xy, CameraBuffer.ProjectionInverse);
		sampleDepth = depthPos.z;

		const float rangeCheck = smoothstep(0.0f, 1.0f, finalRadius / abs(sampleDepth - viewPosition.z));
		occlusion += (sampleDepth >= (samplePos.z - finalBias) ? 0.0f : 1.0f) * rangeCheck;
	}

	occlusion = 1.0f - (occlusion / float(finalKernelSize));
	occlusion = occlusion * occlusion;
	Output[outputTexCoords] = occlusion;

}
