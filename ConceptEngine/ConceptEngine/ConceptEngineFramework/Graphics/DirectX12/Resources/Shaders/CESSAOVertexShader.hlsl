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

Texture2D gNormalMap : register(t0);
Texture2D gDepthMap : register(t1);
Texture2D gRandomVecMap : register(t2);

SamplerState gsamPointClamp : register(s0);
SamplerState gsamLinearClamp : register(s1);
SamplerState gsamDepthMap : register(s2);
SamplerState gsamLinearWrap : register(s3);

static const int gSampleCount = 14;

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
	float3 PosV : POSITION;
	float2 TexC : TEXCOORD0;
};

VertexOut VS(uint vid : SV_VertexID) {
	VertexOut vout;

	vout.TexC = gTexCoords[vid];

	//Quad covering screen in NDC space.
	vout.PosH = float4(2.0f * vout.TexC.x - 1.0f, 1.0f - 2.0f * vout.TexC.y, 0.0f, 1.0f);

	//Transform quad corners to view space near plane
	float4 ph = mul(vout.PosH, gInvProj);
	vout.PosV = ph.xyz / ph.w;

	return vout;
}
