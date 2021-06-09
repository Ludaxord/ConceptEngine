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

float4 FXAASample(in Texture2D Texture, SamplerState Sampler, float2 TexCoord) {
	return Texture.SampleLevel(Sampler, TexCoord, 0.0f);
}

float4 FXAASampleOffset(in Texture2D Texture, in SamplerState Sampler, float2 TexCoord, int2 Offset) {
	return Texture.SampleLevel(Sampler, TexCoord, 0.0f, Offset);
}

float4 FXAASampleOffset(in Texture2D Texture, SamplerState Sampler, float2 TexCoord, int2 Offset) {
	return Texture.SampleLevel(Sampler, TexCoord, 0.0f, Offset);
}

float4 FXAASampleGrad(in Texture2D Texture, in SamplerState Sampler, float2 TexCoord, float2 Grad) {
	return Texture.SampleGrad(Sampler, TexCoord, Grad, Grad);
}

float4 Main(float2 TexCoord : TEXCOORD0) : SV_TARGET0 {
	uint Width;
	uint Height;

	FinalImage.GetDimensions(Width, Height);
	const float2 inverseTextureSize = Float2(1.0f) / float2(Width, Height);

	float4 M = FXAASampleOffset(FinalImage, Sampler, TexCoord, int2(0, 0));
	float LumaM = M.a;
#if PASSTHROUGH
	return M;
#endif

#if DEBUG_LUMINANCE
	return Float4(M.a);
#endif

	float4 N = FXAASampleOffset(FinalImage, Sampler, TexCoord, int2(0, -1));
	float4 S = FXAASampleOffset(FinalImage, Sampler, TexCoord, int2(0, 1));
	float4 W = FXAASampleOffset(FinalImage, Sampler, TexCoord, int2(-1, 0));
	float4 E = FXAASampleOffset(FinalImage, Sampler, TexCoord, int2(1, 0));

	float LumaN = N.a;
	float LumaS = S.a;
	float LumaW = W.a;
	float LumaE = E.a;

	float rangeMin = min(LumaM, min(min(LumaN, LumaS), min(LumaW, LumaE)));
	float rangeMax = max(LumaM, max(max(LumaN, LumaS), max(LumaW, LumaE)));
	float range = rangeMax - rangeMin;
	if (range < max(FXAA_EDGE_THRESHOLD_MIN, rangeMax * FXAA_EDGE_THRESHOLD)) {
#if DEBUG
	return float4(Float3(M.a), 1.0f);
#else
		return float4(M.rgb, 1.0f);
#endif
	}

#if DEBUG_EDGES
	return float(1.0f, 0.0f, 0.0f, 1.0f);
#endif

	float LumaL = (LumaN + LumaS + LumaW + LumaE) * 0.25f;
	float RangeL = abs(LumaL - LumaM);
	float BlendL = max(0.0f, (RangeL / range) - FXAA_SUBPIX_TRIM) * FXAA_SUBPIX_TRIM_SCALE;
	BlendL = min(BlendL, FXAA_SUBPIX_CAP);

#if DEBUG_RANGE
	return Float4(RangeL);
#endif

#if DEBUG_BLEND_FACTOR
	return Float4(BlendL);
#endif

	float NW = FXAASampleOffset(FinalImage, Sampler, TexCoord, int2(-1, -1));
	float SW = FXAASampleOffset(FinalImage, Sampler, TexCoord, int2(-1, 1));
	float NE = FXAASampleOffset(FinalImage, Sampler, TexCoord, int2(1, -1));
	float SE = FXAASampleOffset(FinalImage, Sampler, TexCoord, int2(1, 1));

	float LumaNW = NW.a;
	float LumaNE = NE.a;
	float LumaSW = SW.a;
	float LumaSE = SE.a;

	float3 ColorL = (M.rgb + N.rgb + S.rgb + W.rgb + E.rgb) + (NW.rgb + SW.rgb + NE.rgb + SE.rgb);
	ColorL = ColorL * Float3(1.0f / 9.0f);

	float EdgeVertical = abs((0.25f * LumaNW) + (-0.5f * LumaN) + (0.25f * LumaNE)) +
		abs((0.50f * LumaW) + (-1.0f * LumaM) + (0.50f * LumaE)) +
		abs((0.25f * LumaSW) + (-0.5f * LumaS) + (0.25f * LumaSE));
	float EdgeHorizontal = abs((0.25f * LumaNW) + (-0.5f * LumaW) + (0.25f * LumaSW)) +
		abs((0.50f * LumaN) + (-1.0f * LumaM) + (0.50f * LumaS)) +
		abs((0.25f * LumaNE) + (-0.5f * LumaE) + (0.25f * LumaSE));

	bool isHorizontal = (EdgeHorizontal >= EdgeVertical);

#if DEBUG_HORIZONTAL
	if (isHorizontal) {
		return float4(0.0f, 1.0f, 0.0f, 1.0f);
	} else {
		return float4(0.0f, 0.0f, 1.0f, 1.0f);
	}
#endif

	if (!isHorizontal) {
		LumaN = LumaW;
		LumaS = LumaE;
	}

	float Gradient0 = abs(LumaN - LumaM);
	float Gradient1 = abs(LumaS - LumaM);
	float LumaAvg0 = (LumaN + LumaM) * 0.5f;
	float LumaAvg1 = (LumaS + LumaM) * 0.5f;

	bool Pair0 = (Gradient0 >= Gradient1);
	float LocalLumaAvg = (!Pair0) ? LumaAvg1 : LumaAvg0;
	float LocalGradient = (!Pair0) ? Gradient1 : Gradient0;
	LocalGradient = LocalGradient * FXAA_SEARCH_THRESHOLD;

	float StepLength = isHorizontal ? -inverseTextureSize.y : -inverseTextureSize.x;
	if (!Pair0) {
		StepLength *= -1.0f;
	}

	float2 currentTexCoord = TexCoord;
	if (isHorizontal) {
		currentTexCoord.y += StepLength * 0.5f;
	}
	else {
		currentTexCoord.x += StepLength * 0.5f;
	}

	float2 offset = isHorizontal ? float2(inverseTextureSize.x, 0.0f) : float2(0.0f, inverseTextureSize.y);
	bool Done0 = false;
	bool Done1 = false;
	float LumaEnd0 = LocalLumaAvg;
	float LumaEnd1 = LocalLumaAvg;
	float2 TexCoord0 = currentTexCoord - offset;
	float2 TexCoord1 = currentTexCoord + offset;

	for (int steps = 0; steps < FXAA_SEARCH_STEPS; steps++) {
		if (!Done0) {

		}
		if (!Done1) {

		}

		Done0 = (abs(LumaEnd0 - LocalLumaAvg) >= LocalGradient);
		Done1 = (abs(LumaEnd1 - LocalLumaAvg) >= LocalGradient);
		if (Done0 && Done1) {
			break;
		}

		if (!Done0) {
			TexCoord0 -= offset;
		}

		if (!Done1) {
			TexCoord1 += offset;
		}
	}

	float Distance0 = isHorizontal ? (TexCoord.x - TexCoord0.x) : (TexCoord.y - TexCoord0.y);
	float Distance1 = isHorizontal ? (TexCoord1.x - TexCoord.x) : (TexCoord1.y - TexCoord.y);

	bool Dir0 = Distance0 < Distance1;
#if DEBUG_NEGPOS
	if (Dir0) {
        return float4(1.0f, 0.0f, 0.0f, 1.0f);
    }
    else {
        return float4(0.0f, 0.0f, 1.0f, 1.0f);
    }
#endif

	LumaEnd0 = Dir0 ? LumaEnd0 : LumaEnd1;
	if (((LumaM - LocalLumaAvg) < 0.0f) == ((LumaEnd0 - LocalLumaAvg) < 0.0f)) {
		StepLength = 0.0f;
	}

#if DEBUG_STEP
	return Float4(StepLength);
#endif

	float spanLength = (Distance0 + Distance1);
	Distance0 = Dir0 ? Distance0 : Distance1;

#if DEBUG_STEP
	return Float4(StepLength);
#endif

	float subPixelOffset = (0.5f + (Distance0 * (-1.0f / spanLength))) * spanLength;
	float finalTexCoord = TexCoord + float2(isHorizontal ? 0.0f : subPixelOffset, isHorizontal ? subPixelOffset : 0.0f);
	float3 ColorF = FXAASample(FinalImage, Sampler, finalTexCoord).rgb;
	float3 FinalColor = Lerp(ColorL, ColorF, BlendL);
	return float4(FinalColor, 1.0f);
}
