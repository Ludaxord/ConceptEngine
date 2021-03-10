// Defaults for number of lights.
#ifndef NUM_DIR_LIGHTS
#define NUM_DIR_LIGHTS 3
#endif

#ifndef NUM_POINT_LIGHTS
#define NUM_POINT_LIGHTS 0
#endif

#ifndef NUM_SPOT_LIGHTS
#define NUM_SPOT_LIGHTS 0
#endif

// Include structures and functions for lighting.
#include "LightUtil.hlsl"
#include "VertexHeader.h"

Texture2D gDiffuseMap : register(t0);
Texture2D gDisplacementMap : register(t1);

SamplerState gsamPointWrap : register(s0);
SamplerState gsamPointClamp : register(s1);
SamplerState gsamLinearWrap : register(s2);
SamplerState gsamLinearClamp : register(s3);
SamplerState gsamAnisotropicWrap : register(s4);
SamplerState gsamAnisotropicClamp : register(s5);

// Constant data that varies per frame.
cbuffer cbPerObject : register(b0)
{
float4x4 gWorld;
float4x4 gTexTransform;
float2 gDisplacementMapTexelSize;
float gGridSpatialStep;
float cbPerObjectPad1;
};

// Constant data that varies per pass.
cbuffer cbPass : register(b1)
{
float4x4 gView;
float4x4 gInvView;
float4x4 gProj;
float4x4 gInvProj;
float4x4 gViewProj;
float4x4 gInvViewProj;
float3 gEyePosW;
float cbPerPassPad1;
float2 gRenderTargetSize;
float2 gInvRenderTargetSize;
float gNearZ;
float gFarZ;
float gTotalTime;
float gDeltaTime;
float4 gAmbientLight;

// Allow application to change fog parameters once per frame.
// For example, we may only use fog for certain times of day.
float4 gFogColor;
float gFogStart;
float gFogRange;
float2 cbPerPassPad2;

// Indices [0, NUM_DIR_LIGHTS) are directional lights;
// indices [NUM_DIR_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHTS) are point lights;
// indices [NUM_DIR_LIGHTS+NUM_POINT_LIGHTS, NUM_DIR_LIGHTS+NUM_POINT_LIGHT+NUM_SPOT_LIGHTS)
// are spot lights for a maximum of MaxLights per object.
Light gLights[MaxLights];
};

cbuffer cbMaterial : register(b2)
{
float4 gDiffuseAlbedo;
float3 gFresnelR0;
float gRoughness;
float4x4 gMatTransform;
};

LitTexVertexOut VS(LitTexVertexIn vIn) {
	LitTexVertexOut vOut = (LitTexVertexOut)0.0f;

#ifdef DISPLACEMENT_MAP
	// Sample the displacement map using non-transformed [0,1]^2 tex-coords.
	vIn.PosL.y += gDisplacementMap.SampleLevel(gsamLinearWrap, vIn.TexCoord, 1.0f).r;

	// Estimate normal using finite difference.
	float du = gDisplacementMapTexelSize.x;
	float dv = gDisplacementMapTexelSize.y;
	float l = gDisplacementMap.SampleLevel(gsamPointClamp, vIn.TexCoord - float2(du, 0.0f), 0.0f).r;
	float r = gDisplacementMap.SampleLevel(gsamPointClamp, vIn.TexCoord + float2(du, 0.0f), 0.0f).r;
	float t = gDisplacementMap.SampleLevel(gsamPointClamp, vIn.TexCoord - float2(0.0f, dv), 0.0f).r;
	float b = gDisplacementMap.SampleLevel(gsamPointClamp, vIn.TexCoord + float2(0.0f, dv), 0.0f).r;
	vIn.NormalL = normalize(float3(-r + l, 2.0f * gGridSpatialStep, b - t));

#endif

	//Transform to world space
	float4 posW = mul(float4(vIn.PosL, 1.0f), gWorld);
	vOut.PosW = posW.xyz;

	//Assumes nonuniform scaling; otherwise, need to use inverse-transpose of world matrix
	vOut.NormalW = mul(vIn.NormalL, (float3x3)gWorld);

	//Transform to homogeneous clip space
	vOut.PosH = mul(posW, gViewProj);

	//Output vertex attributes for interpolation across triangle
	float4 texC = mul(float4(vIn.TexCoord, 0.0f, 1.0f), gTexTransform);
	vOut.TexCoord = mul(texC, gMatTransform).xy;

	return vOut;
}
