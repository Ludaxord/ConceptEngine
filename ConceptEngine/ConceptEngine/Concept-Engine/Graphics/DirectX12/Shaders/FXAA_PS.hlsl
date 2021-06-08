#include "Helpers.hlsli"
#include "Constants.hlsli"

#ifdef ENABLE_DEBUG
#define DEBUG_LUMINANCE 0
#define DEBUG_EDGES 1
#define PASSTHROUGH 0
#define DEBUG 0
#define DEBUG_HORIZONTAL 0
#define DEBUG_NEGPOS 0
#define DEBUG_STEP 0
#define DEBUG_BLEND_FACTOR 0
#define DEBUG_RANGE 0
#endif

// FXAA Settings
#define FXAA_EDGE_THRESHOLD		(1.0f / 8.0f)
#define FXAA_EDGE_THRESHOLD_MIN	(1.0f / 24.0f)
#define FXAA_SUBPIX_TRIM		(1.0f / 4.0f)
#define FXAA_SUBPIX_CAP			(3.0f / 4.0f)
#define FXAA_SUBPIX_TRIM_SCALE	(1.0f / (1.0f - FXAA_SUBPIX_TRIM))
#define FXAA_SEARCH_THRESHOLD	(1.0f / 4.0f)
#define FXAA_SEARCH_STEPS		24

cbuffer CB0 : register(b0, D3D12_SHADER_REGISTER_SPACE_32BIT_CONSTANTS) {
float2 TextureSize;
}

Texture2D FinalImage : register(t0, space0);
SamplerState Sampler : register(s0, space0);

float4 FXAASample(in Texture2D Texture, in SamplerState Sampler, float2 TexCoord) {
	return Texture.SampleLevel(Sampler, TexCoord, 0.0f);
}

float4 FXAASampleOffset(in Texture2D Texture, in SamplerState Sampler, float2 TexCoord, int2 Offset) {
	return Texture.SampleLevel(Sampler, TexCoord, 0.0f, Offset);
}

float4 FXAASampleGrad(in Texture2D Texture, in SamplerState Sampler, float2 TexCoord, float2 Grad) {
	return Texture.SampleGrad(Sampler, TexCoord, Grad, Grad);
}

float4 Main(float2 TexCoord : TEXCOORD0) : SV_TARGET0 {

}
