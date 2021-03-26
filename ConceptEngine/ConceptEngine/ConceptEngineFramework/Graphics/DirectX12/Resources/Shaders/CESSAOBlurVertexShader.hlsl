/*
 * Performs a bilateral edge preserving blur of ambient map.
 * We use a pixel shader instead of compute shader to avoid the switch from compute mode to rendering mode.
 * The texture cache makes up for some of the loss of not having shared memory.
 * The ambient map uses 16-bit texture format, which is small, so we should be able to fit a lot of texels in cache.
 */

cbuffer cbSsao : register(b0) {
float4x4 gProj;
float4x4 gInvProj;
float4x4 gProjTex;
float4 gOffsetVectors[14];

//For SSAO Blur
float4 gBlurWeights[3];

float2 gInvRenderTargetSize;

//Coordinates given in view space
float gOcclusionRadius;
float gOcclusionFadeStart;
float gOcclusionFadeEnd;
float gSurfaceEpsilon;
}

cbuffer cbRootConstants : register(b1) {
bool gHorizontalBlur;
}

//Nonnumeric values cannot by added to cbuffer
Texture2D gNormalMap : register(t0);
Texture2D gDepthMap : register(t1);
Texture2D gInputMap : register(t2);

SamplerState gsamPointClamp : register(s0);
SamplerState gsamLinearClamp : register(s1);
SamplerState gsamDepthMap : register(s2);
SamplerState gsamLinearWrap : register(s3);

static const int gBlurRadius = 5;

static const float2 gTexCoords[6] = {
	float2(0.0f, 1.0f),
	float2(0.0f, 0.0f),
	float2(1.0f, 0.0f),
	float2(0.0f, 1.0f),
	float2(1.0f, 0.0f),
	float2(1.0f, 1.0f)
};


struct VertexOut {
	float4 PosH : SV_POSITION;
	float2 TexC : TEXCOORD;
};

VertexOut VS(uint vid : SV_VertexID) {
	VertexOut vout;
	vout.TexC = gTexCoords[vid];

	//Quad covering screen in NDC space.
	vout.PosH = float4(2.0f * vout.TexC.x - 1.0f, 1.0f - 2.0f * vout.TexC.y, 0.0f, 1.0f);

	return vout;
}
